#include "stdafx.h"
#include "Graphics.h"
#include "ShaderBufferTypes.h"
#include "SamplerStateDescConstants.h"

using namespace DirectX;
using namespace GraphicsEngine;

Graphics::Graphics(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight, bool fullscreen) :
	m_d3dBase(outputWindow, clientWidth, clientHeight, fullscreen),
	m_pipelineStateManager(m_d3dBase),
	m_camera(m_d3dBase.GetAspectRatio(), 0.25f * XM_PI, 1.0f, 1000.0f, XMMatrixIdentity()),
	m_scene(this, m_d3dBase, m_textureManager),
	m_frameResources(1, FrameResource(m_d3dBase.GetDevice(), m_allRenderItems, m_scene.GetMaterials().size())),
	m_currentFrameResource(&m_frameResources[0]),
	m_anisotropicSamplerState(m_d3dBase.GetDevice(), SamplerStateDescConstants::AnisotropicClamp)
{
	m_d3dBase.SetClearColor(XMFLOAT3(0.2f, 0.2f, 0.2f));
	m_camera.SetPosition(0.0f, 2.5f, -15.0f);
}

void Graphics::OnResize(uint32_t clientWidth, uint32_t clientHeight)
{
	// TODO
}

void Graphics::Update(const Common::Timer& timer)
{
	m_camera.Update();
	UpdatePassData(timer);
	UpdateMaterialData();
	UpdateObjectsData();
}

void Graphics::Render(const Common::Timer& timer) const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	m_d3dBase.BeginScene();

	// Set pass data:
	deviceContext->VSSetConstantBuffers(2, 1, m_currentFrameResource->PassData.GetAddressOf());
	deviceContext->PSSetConstantBuffers(2, 1, m_currentFrameResource->PassData.GetAddressOf());

	// Set sampler:
	deviceContext->PSSetSamplers(5, 1, m_anisotropicSamplerState.GetAddressOf());

	m_pipelineStateManager.SetPipelineState(deviceContext, "Standard");
	DrawRenderItems(RenderLayer::Opaque);

	m_d3dBase.EndScene();
}

Camera* Graphics::GetCamera()
{
	return &m_camera;
}

void Graphics::AddRenderItem(std::unique_ptr<RenderItem>&& renderItem, std::initializer_list<RenderLayer> renderLayers)
{
	for (auto renderLayer : renderLayers)
	{
		m_renderItemLayers[static_cast<SIZE_T>(renderLayer)].push_back(renderItem.get());
	}

	m_allRenderItems.push_back(std::move(renderItem));
}

void Graphics::UpdateObjectsData()
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	for(auto& renderItem : m_allRenderItems)
	{
		// TODO optimize
	
		ShaderBufferTypes::ObjectData objectBuffer;

		// Transpose and store world matrix:
		XMStoreFloat4x4(&objectBuffer.WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&renderItem->WorldMatrix)));

		// Update buffer:
		m_currentFrameResource->ObjectDataArray[renderItem->ObjectBufferIndex].Map(deviceContext, &objectBuffer, sizeof(ShaderBufferTypes::ObjectData));
	}
}
void Graphics::UpdateMaterialData() const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	for(auto& e : m_scene.GetMaterials())
	{
		auto material = e.second.get();

		auto materialTransform = XMLoadFloat4x4(&material->MaterialTransform);

		ShaderBufferTypes::MaterialData materialData;
		materialData.DiffuseAlbedo = material->DiffuseAlbedo;
		materialData.FresnelR0 = material->FresnelR0;
		materialData.Roughness = material->Roughness;
		XMStoreFloat4x4(&materialData.MaterialTransform, XMMatrixTranspose(materialTransform));

		m_currentFrameResource->MaterialDataArray[material->MaterialIndex].Map(deviceContext, &materialData, sizeof(ShaderBufferTypes::MaterialData));
	}
}
void Graphics::UpdatePassData(const Common::Timer& timer) const
{
	ShaderBufferTypes::PassData passData;

	auto viewMatrix = m_camera.GetViewMatrix();
	auto viewMatrixDeterminant = XMMatrixDeterminant(viewMatrix);
	auto inverseViewMatrix = XMMatrixInverse(&viewMatrixDeterminant, viewMatrix);

	auto projectionMatrix = m_camera.GetProjectionMatrix();
	auto projectionMatrixDeterminant = XMMatrixDeterminant(projectionMatrix);
	auto inverseProjectionMatrix = XMMatrixInverse(&projectionMatrixDeterminant, projectionMatrix);

	auto viewProjectionMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);
	auto viewProjectionMatrixDeterminant = XMMatrixDeterminant(viewProjectionMatrix);
	auto inverseViewProjectionMatrix = XMMatrixInverse(&viewProjectionMatrixDeterminant, viewProjectionMatrix);

	XMStoreFloat4x4(&passData.ViewMatrix, XMMatrixTranspose(viewMatrix));
	XMStoreFloat4x4(&passData.InverseViewMatrix, XMMatrixTranspose(inverseViewMatrix));
	XMStoreFloat4x4(&passData.ProjectionMatrix, XMMatrixTranspose(projectionMatrix));
	XMStoreFloat4x4(&passData.InverseProjectionMatrix, XMMatrixTranspose(inverseProjectionMatrix));
	XMStoreFloat4x4(&passData.ViewProjectionMatrix, XMMatrixTranspose(viewProjectionMatrix));
	XMStoreFloat4x4(&passData.InverseProjectionMatrix, XMMatrixTranspose(inverseViewProjectionMatrix));
	XMStoreFloat3(&passData.EyePositionW, m_camera.GetPosition());
	passData.RenderTargetSize = XMFLOAT2(static_cast<float>(m_d3dBase.GetClientWidth()), static_cast<float>(m_d3dBase.GetClientHeight()));
	passData.InverseRenderTargetSize = XMFLOAT2(1.0f / static_cast<float>(m_d3dBase.GetClientWidth()), 1.0f / static_cast<float>(m_d3dBase.GetClientHeight()));
	passData.NearZ = 1.0f;
	passData.FarZ = 1000.0f;
	passData.TotalTime = static_cast<float>(timer.GetTotalMilliseconds());
	passData.DeltaTime = static_cast<float>(timer.GetDeltaMilliseconds());
	passData.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
	passData.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
	passData.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
	passData.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	passData.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
	passData.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
	passData.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

	m_currentFrameResource->PassData.Map(m_d3dBase.GetDeviceContext(), &passData, sizeof(ShaderBufferTypes::PassData));
}

void Graphics::DrawRenderItems(RenderLayer renderLayer) const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	// For each render item:
	for(auto& renderItem : m_renderItemLayers[static_cast<SIZE_T>(renderLayer)])
	{
		// Set object data:
		const auto& objectData = m_currentFrameResource->ObjectDataArray[renderItem->ObjectBufferIndex];
		deviceContext->VSSetConstantBuffers(0, 1, objectData.GetAddressOf());

		// Set material data:
		const auto& materialData = m_currentFrameResource->MaterialDataArray[renderItem->Material->MaterialIndex];
		deviceContext->PSSetConstantBuffers(1, 1, materialData.GetAddressOf());

		// Set textures:
		deviceContext->PSSetShaderResources(0, 1, renderItem->Material->DiffuseMap->GetAddressOf());

		// Render:
		renderItem->Render(deviceContext);
	}
}

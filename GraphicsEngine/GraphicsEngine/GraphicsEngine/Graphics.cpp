﻿#include "stdafx.h"
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
	UpdateInstancesData();
}

void Graphics::Render(const Common::Timer& timer) const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	m_d3dBase.BeginScene();

	// Set pass data:
	deviceContext->VSSetConstantBuffers(2, 1, m_currentFrameResource->PassData.GetAddressOf());
	deviceContext->PSSetConstantBuffers(2, 1, m_currentFrameResource->PassData.GetAddressOf());

	// Set samplers:
	deviceContext->PSSetSamplers(5, 1, m_anisotropicSamplerState.GetAddressOf());

	// Draw opaque:
	m_pipelineStateManager.SetPipelineState(deviceContext, "Opaque");
	DrawRenderItems(RenderLayer::Opaque);

	// Draw transparent:
	m_pipelineStateManager.SetPipelineState(deviceContext, "Transparent");
	DrawRenderItems(RenderLayer::Transparent);

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

void Graphics::UpdateInstancesData()
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	for(const auto& renderItem : m_allRenderItems)
	{
		// TODO optimize
	
		// Get instances buffer for the current render item:
		auto instancesBuffer = m_currentFrameResource->InstancesBuffers.at(renderItem->Name);

		// Map resource:
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		instancesBuffer.Map(deviceContext, &mappedResource);
		auto instacesBufferView = reinterpret_cast<ShaderBufferTypes::InstanceData*>(mappedResource.pData);

		// For each instance:
		for(SIZE_T i = 0; i < renderItem->InstancesData.size(); ++i)
		{
			const auto& instance = renderItem->InstancesData[i];

			// Get view to the instance data:
			auto& instanceData = instacesBufferView[i];

			// Transpose and store world matrix:
			XMStoreFloat4x4(&instanceData.WorldMatrix, XMLoadFloat4x4(&instance.WorldMatrix));
		}

		// Unmap resource:
		instancesBuffer.Unmap(deviceContext);
	}
}
void Graphics::UpdateMaterialData() const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	for(auto& e : m_scene.GetMaterials())
	{
		auto material = e.second.get();

		ShaderBufferTypes::MaterialData materialData;
		materialData.DiffuseAlbedo = material->DiffuseAlbedo;
		materialData.FresnelR0 = material->FresnelR0;
		materialData.Roughness = material->Roughness;
		XMStoreFloat4x4(&materialData.MaterialTransform, XMMatrixTranspose(XMLoadFloat4x4(&material->MaterialTransform)));

		m_currentFrameResource->MaterialDataArray[material->MaterialIndex].CopyData(deviceContext, &materialData, sizeof(ShaderBufferTypes::MaterialData));
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

	m_currentFrameResource->PassData.CopyData(m_d3dBase.GetDeviceContext(), &passData, sizeof(ShaderBufferTypes::PassData));
}

void Graphics::DrawRenderItems(RenderLayer renderLayer) const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	UINT stride = sizeof(ShaderBufferTypes::InstanceData);
	UINT offset = 0;

	// For each render item:
	for(auto& renderItem : m_renderItemLayers[static_cast<SIZE_T>(renderLayer)])
	{
		// Set instances data:
		deviceContext->IASetVertexBuffers(1, 1, m_currentFrameResource->InstancesBuffers[renderItem->Name].GetAddressOf(), &stride, &offset);

		// Set material data:
		const auto& materialData = m_currentFrameResource->MaterialDataArray[renderItem->Material->MaterialIndex];
		deviceContext->VSSetConstantBuffers(1, 1, materialData.GetAddressOf());
		deviceContext->PSSetConstantBuffers(1, 1, materialData.GetAddressOf());

		// Set textures:
		deviceContext->PSSetShaderResources(0, 1, renderItem->Material->DiffuseMap->GetAddressOf());

		// Render:
		renderItem->Render(deviceContext);
	}
}

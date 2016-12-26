#include "stdafx.h"
#include "Graphics.h"
#include "ShaderBufferTypes.h"
#include "SamplerStateDescConstants.h"
#include "Terrain.h"

using namespace DirectX;
using namespace GraphicsEngine;

Graphics::Graphics(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight, bool fullscreen) :
	m_d3dBase(outputWindow, clientWidth, clientHeight, fullscreen),
	m_pipelineStateManager(m_d3dBase),
	m_camera(m_d3dBase.GetAspectRatio(), 0.25f * XM_PI, 0.01f, 10000.0f, XMMatrixIdentity()),
	m_scene(this, m_d3dBase, m_textureManager, m_lightManager),
	m_frameResources(1, FrameResource(m_d3dBase.GetDevice(), m_allRenderItems, m_scene.GetMaterials().size())),
	m_currentFrameResource(&m_frameResources[0]),
	m_linearClampSamplerState(m_d3dBase.GetDevice(), SamplerStateDescConstants::LinearClamp),
	m_anisotropicWrapSamplerState(m_d3dBase.GetDevice(), SamplerStateDescConstants::AnisotropicWrap),
	m_anisotropicClampSamplerState(m_d3dBase.GetDevice(), SamplerStateDescConstants::AnisotropicClamp),
	m_fog(false),
	m_fogColor(0.5f, 0.5f, 0.5f),
	m_shadowMap(m_d3dBase.GetDevice(), clientWidth, clientHeight)
{
	m_d3dBase.SetClearColor(m_fogColor);
	m_camera.SetPosition(0.0f, 0.0f, 0.0f);
}

void Graphics::OnResize(uint32_t clientWidth, uint32_t clientHeight)
{
	// TODO
}

void Graphics::Update(const Common::Timer& timer)
{
	m_camera.Update();
	UpdateMainPassData(timer);
	UpdateShadowPassData(timer);
	UpdateMaterialData();
	UpdateInstancesData();
}

void Graphics::Render(const Common::Timer& timer) const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	m_d3dBase.BeginScene();

	// Set samplers:
	deviceContext->DSSetSamplers(3, 1, m_linearClampSamplerState.GetAddressOf());
	deviceContext->PSSetSamplers(3, 1, m_linearClampSamplerState.GetAddressOf());
	deviceContext->DSSetSamplers(4, 1, m_anisotropicWrapSamplerState.GetAddressOf());
	deviceContext->PSSetSamplers(4, 1, m_anisotropicWrapSamplerState.GetAddressOf());
	deviceContext->DSSetSamplers(5, 1, m_anisotropicClampSamplerState.GetAddressOf());
	deviceContext->PSSetSamplers(5, 1, m_anisotropicClampSamplerState.GetAddressOf());

	// Create shadow map:
	{
		// Set main pass data:
		deviceContext->VSSetConstantBuffers(2, 1, m_currentFrameResource->ShadowPassData.GetAddressOf());
		deviceContext->HSSetConstantBuffers(2, 1, m_currentFrameResource->ShadowPassData.GetAddressOf());
		deviceContext->DSSetConstantBuffers(2, 1, m_currentFrameResource->ShadowPassData.GetAddressOf());
		deviceContext->PSSetConstantBuffers(2, 1, m_currentFrameResource->ShadowPassData.GetAddressOf());

		// Set a new depth stencil:
		m_shadowMap.SetDepthStencilView(deviceContext);

		// Draw opaque:
		m_pipelineStateManager.SetPipelineState(deviceContext, "OpaqueShadow");
		DrawRenderItems(RenderLayer::Opaque);

		// Draw terrain:
		m_pipelineStateManager.SetPipelineState(deviceContext, "TerrainShadow");
		DrawTerrain();

		m_d3dBase.SetDefaultRenderTargets();

		// Draw transparent:
		/*m_pipelineStateManager.SetPipelineState(deviceContext, "TransparentShadow");
		DrawRenderItems(RenderLayer::Transparent);

		// Draw alpha-clipped:
		m_pipelineStateManager.SetPipelineState(deviceContext, "AlphaClippedShadow");
		DrawRenderItems(RenderLayer::AlphaClipped);*/
	}

	// Set main pass data:
	deviceContext->VSSetConstantBuffers(2, 1, m_currentFrameResource->MainPassData.GetAddressOf());
	deviceContext->HSSetConstantBuffers(2, 1, m_currentFrameResource->MainPassData.GetAddressOf());
	deviceContext->DSSetConstantBuffers(2, 1, m_currentFrameResource->MainPassData.GetAddressOf());
	deviceContext->PSSetConstantBuffers(2, 1, m_currentFrameResource->MainPassData.GetAddressOf());

	if(!m_fog)
	{
		// Draw Skydome
		m_pipelineStateManager.SetPipelineState(deviceContext, "SkyDome");
		DrawRenderItems(RenderLayer::SkyDome);

		// Draw opaque:
		m_pipelineStateManager.SetPipelineState(deviceContext, "Opaque");
		DrawRenderItems(RenderLayer::Opaque);

		// Draw terrain:
		m_pipelineStateManager.SetPipelineState(deviceContext, "Terrain");
		DrawTerrain();

		// Draw transparent:
		m_pipelineStateManager.SetPipelineState(deviceContext, "Transparent");
		DrawRenderItems(RenderLayer::Transparent);

		// Draw alpha-clipped:
		m_pipelineStateManager.SetPipelineState(deviceContext, "AlphaClipped");
		DrawRenderItems(RenderLayer::AlphaClipped);
	}
	else
	{
		// Draw opaque:
		m_pipelineStateManager.SetPipelineState(deviceContext, "OpaqueFog");
		DrawRenderItems(RenderLayer::Opaque);

		// Draw terrain:
		m_pipelineStateManager.SetPipelineState(deviceContext, "TerrainFog");
		DrawTerrain();

		// Draw transparent:
		m_pipelineStateManager.SetPipelineState(deviceContext, "TransparentFog");
		DrawRenderItems(RenderLayer::Transparent);

		// Draw alpha-clipped:
		m_pipelineStateManager.SetPipelineState(deviceContext, "AlphaClippedFog");
		DrawRenderItems(RenderLayer::AlphaClipped);
	}
	
	m_shadowMap.ClearDepthStencilView(deviceContext);

	m_d3dBase.EndScene();
}

Camera* Graphics::GetCamera()
{
	return &m_camera;
}
IScene* Graphics::GetScene()
{
	return &m_scene;
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

	// Get view matrix and calculate its inverse:
	auto viewMatrix = m_camera.GetViewMatrix();
	auto viewMatrixDeterminant = XMMatrixDeterminant(viewMatrix);
	auto inverseViewMatrix = XMMatrixInverse(&viewMatrixDeterminant, viewMatrix);

	// Build the view space camera frustum:
	auto viewSpaceCameraFrustum = m_camera.BuildViewSpaceBoundingFrustum();

	for(const auto& renderItem : m_allRenderItems)
	{
		// Get instances buffer for the current render item:
		auto instancesBuffer = m_currentFrameResource->InstancesBuffers.at(renderItem->Name);

		// Map resource:
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		instancesBuffer.Map(deviceContext, &mappedResource);
		auto instacesBufferView = reinterpret_cast<ShaderBufferTypes::InstanceData*>(mappedResource.pData);

		// For each instance:
		auto visibleInstanceCount = 0;
		for(const auto& instanceData : renderItem->InstancesData)
		{
			// Get the world matrix of the instance and calculate its inverse:
			auto worldMatrix = XMLoadFloat4x4(&instanceData.WorldMatrix);
			auto worldMatrixDeterminant = XMMatrixDeterminant(worldMatrix);
			auto inverseWorldMatrix = XMMatrixInverse(&worldMatrixDeterminant, worldMatrix);

			// Calculate the matrix which transforms from view space to the instance's local space:
			auto viewToLocalMatrix = XMMatrixMultiply(inverseViewMatrix, inverseWorldMatrix);

			// Transform camera frustum from view space to local space:
			BoundingFrustum localSpaceCameraFrustum;
			viewSpaceCameraFrustum.Transform(localSpaceCameraFrustum, viewToLocalMatrix);

			// If the camera frustum intersects the instance bounds:
			if(localSpaceCameraFrustum.Contains(renderItem->Bounds) != ContainmentType::DISJOINT)
			{
				// Update instance data:
				auto& instanceDataView = instacesBufferView[visibleInstanceCount++];
				instanceDataView.WorldMatrix = instanceData.WorldMatrix;
			}
		}
		renderItem->VisibleInstanceCount = visibleInstanceCount;

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
void Graphics::UpdateMainPassData(const Common::Timer& timer) const
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
	passData.TerrainDisplacementScalarY = 1.0f;
	passData.RenderTargetSize = XMFLOAT2(static_cast<float>(m_d3dBase.GetClientWidth()), static_cast<float>(m_d3dBase.GetClientHeight()));
	passData.InverseRenderTargetSize = XMFLOAT2(1.0f / static_cast<float>(m_d3dBase.GetClientWidth()), 1.0f / static_cast<float>(m_d3dBase.GetClientHeight()));
	passData.NearZ = m_camera.GetNearZ();
	passData.FarZ = m_camera.GetFarZ();
	passData.TotalTime = static_cast<float>(timer.GetTotalMilliseconds());
	passData.DeltaTime = static_cast<float>(timer.GetDeltaMilliseconds());
	passData.FogColor = XMFLOAT4(m_fogColor.x, m_fogColor.y, m_fogColor.z, 1.0f);
	passData.FogStart = 20.0f;
	passData.FogRange = 100.0f;
	passData.MaxTesselationDistance = 100.0f;
	passData.MaxTesselationFactor = 6.0f;
	passData.MinTesselationDistance = 500.0f;
	passData.MinTesselationFactor = 1.0f;
	
	const auto& terrain = m_scene.GetTerrain();
	passData.TexelSize = terrain.GetTexelSize();
	passData.TiledTexelScale = terrain.GetDescription().TiledTexelScale;
	
	passData.SkyDomeColors[0] = { 0.5f, 0.1f, 0.1f, 1.0f };
	passData.SkyDomeColors[1] = { 0.1f, 0.1f, 0.8f, 1.0f };
	passData.AmbientLight = m_lightManager.GetAmbientLight();
	m_lightManager.Fill(passData.Lights.begin(), passData.Lights.end());

	m_currentFrameResource->MainPassData.CopyData(m_d3dBase.GetDeviceContext(), &passData, sizeof(ShaderBufferTypes::PassData));
}
void Graphics::UpdateShadowPassData(const Common::Timer& timer) const
{
	ShaderBufferTypes::PassData passData;

	auto castShadowsLights = m_lightManager.GetCastShadowsLights();
	auto castShadowsLight = castShadowsLights[0];

	auto viewMatrixFloat = castShadowsLight->GetViewMatrix();
	auto viewMatrix = XMLoadFloat4x4(&viewMatrixFloat);
	auto viewMatrixDeterminant = XMMatrixDeterminant(viewMatrix);
	auto inverseViewMatrix = XMMatrixInverse(&viewMatrixDeterminant, viewMatrix);

	auto projectionMatrixFloat = castShadowsLight->GetOrthographicMatrix(m_d3dBase.GetClientWidth(), m_d3dBase.GetClientHeight(), m_camera.GetNearZ(), m_camera.GetFarZ());
	auto projectionMatrix = XMLoadFloat4x4(&projectionMatrixFloat);
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
	passData.TerrainDisplacementScalarY = 1.0f;
	passData.RenderTargetSize = XMFLOAT2(static_cast<float>(m_d3dBase.GetClientWidth()), static_cast<float>(m_d3dBase.GetClientHeight()));
	passData.InverseRenderTargetSize = XMFLOAT2(1.0f / static_cast<float>(m_d3dBase.GetClientWidth()), 1.0f / static_cast<float>(m_d3dBase.GetClientHeight()));
	passData.NearZ = m_camera.GetNearZ();
	passData.FarZ = m_camera.GetFarZ();
	passData.TotalTime = static_cast<float>(timer.GetTotalMilliseconds());
	passData.DeltaTime = static_cast<float>(timer.GetDeltaMilliseconds());
	passData.FogColor = XMFLOAT4(m_fogColor.x, m_fogColor.y, m_fogColor.z, 1.0f);
	passData.FogStart = 20.0f;
	passData.FogRange = 100.0f;
	passData.MaxTesselationDistance = 100.0f;
	passData.MaxTesselationFactor = 6.0f;
	passData.MinTesselationDistance = 500.0f;
	passData.MinTesselationFactor = 1.0f;

	const auto& terrain = m_scene.GetTerrain();
	passData.TexelSize = terrain.GetTexelSize();
	passData.TiledTexelScale = terrain.GetDescription().TiledTexelScale;

	passData.SkyDomeColors[0] = { 0.5f, 0.1f, 0.1f, 1.0f };
	passData.SkyDomeColors[1] = { 0.1f, 0.1f, 0.8f, 1.0f };
	passData.AmbientLight = m_lightManager.GetAmbientLight();
	m_lightManager.Fill(passData.Lights.begin(), passData.Lights.end());

	m_currentFrameResource->ShadowPassData.CopyData(m_d3dBase.GetDeviceContext(), &passData, sizeof(ShaderBufferTypes::PassData));
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
		if(renderItem->Material->DiffuseMap != nullptr)
			deviceContext->PSSetShaderResources(0, 1, renderItem->Material->DiffuseMap->GetAddressOf());

		// Render:
		renderItem->Render(deviceContext);
	}
}

void Graphics::DrawTerrain() const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	UINT stride = sizeof(ShaderBufferTypes::InstanceData);
	UINT offset = 0;

	// For each render item:
	for (auto& renderItem : m_renderItemLayers[static_cast<SIZE_T>(RenderLayer::Terrain)])
	{
		// Set instances data:
		deviceContext->IASetVertexBuffers(1, 1, m_currentFrameResource->InstancesBuffers[renderItem->Name].GetAddressOf(), &stride, &offset);

		// Set material data:
		const auto& materialData = m_currentFrameResource->MaterialDataArray[renderItem->Material->MaterialIndex];
		deviceContext->VSSetConstantBuffers(1, 1, materialData.GetAddressOf());
		deviceContext->PSSetConstantBuffers(1, 1, materialData.GetAddressOf());

		// Set textures:
		deviceContext->PSSetShaderResources(0, 1, renderItem->Material->DiffuseMap->GetAddressOf());
		deviceContext->PSSetShaderResources(1, 1, renderItem->Material->NormalMap->GetAddressOf());
		deviceContext->PSSetShaderResources(2, 1, renderItem->Material->HeightMap->GetAddressOf());
		deviceContext->DSSetShaderResources(2, 1, renderItem->Material->HeightMap->GetAddressOf());

		// Render:
		renderItem->Render(deviceContext);
	}
}

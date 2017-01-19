#include "stdafx.h"
#include "Graphics.h"
#include "ShaderBufferTypes.h"
#include "SamplerStateDescConstants.h"
#include "Terrain.h"
#include "Common/PerformanceTimer.h"
#include <numeric>

using namespace DirectX;
using namespace GraphicsEngine;

Graphics::Graphics(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight, bool fullscreen) :
	m_initialized(false),
	m_d3dBase(outputWindow, clientWidth, clientHeight, fullscreen),
	m_pipelineStateManager(m_d3dBase),
	m_camera(m_d3dBase.GetAspectRatio(), 0.25f * XM_PI, 0.01f, 10000.0f, XMMatrixIdentity()),
	m_lightManager(),
	m_octree(32, BoundingBox(XMFLOAT3(0.0f, 256.0f, 0.0f), XMFLOAT3(1024.0f, 512.0f, 1024.0f)), XMFLOAT3(64.0f, 64.0f, 64.0f)),
	m_scene(this, m_d3dBase, m_textureManager, m_lightManager),
	m_frameResources(1, FrameResource(m_d3dBase.GetDevice(), m_normalRenderItems, m_scene.GetMaterials().size())),
	m_currentFrameResource(&m_frameResources[0]),
	m_linearClampSamplerState(m_d3dBase.GetDevice(), SamplerStateDescConstants::LinearClamp),
	m_anisotropicWrapSamplerState(m_d3dBase.GetDevice(), SamplerStateDescConstants::AnisotropicWrap),
	m_anisotropicClampSamplerState(m_d3dBase.GetDevice(), SamplerStateDescConstants::AnisotropicClamp),
	m_shadowsSamplerState(m_d3dBase.GetDevice(), SamplerStateDescConstants::Shadows),
	m_fog(false),
	m_shadowMap(m_d3dBase.GetDevice(), 2048, 2048),
	m_renderTexture(m_d3dBase.GetDevice(), clientWidth, clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM),
	m_sceneBounds(XMFLOAT3(0.0f, 256.0f, 0.0f), 512.0f),
	m_visibleInstances(0),
	m_debugWindowMode(DebugMode::Hidden),
	m_enableShadows(true),
	m_drawTerrainOnly(false),
	m_cubeMapSkipFramesCurrentCount(0)
{
	//m_camera.SetPosition(220.0f - 512.0f, 27.0f, -(0.0f - 512.0f));
	m_camera.SetPosition(-372.0f, 24.0f, 308.0f);
	m_camera.Update();
	m_camera.RotateWorldY(XM_PI);

	SetupDebugMode();
	InitializeMainPassData();
	BindSamplers();
	//SetupTerrainMeshData();

	SetFogColor(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));

	m_initialized = true;
}

void Graphics::OnResize(uint32_t clientWidth, uint32_t clientHeight)
{
	// TODO
}

void Graphics::FixedUpdate(const Common::Timer& timer)
{
	auto deltaSeconds = static_cast<float>(timer.GetMillisecondsPerUpdate()) / 1000.0f;

	const auto& castShadowsLights = m_lightManager.GetCastShadowsLights();
	auto castShadowsLight = castShadowsLights[0];

	auto deltaYaw = deltaSeconds * XM_PI / 48.0f;
	castShadowsLight->RotateRollPitchYaw(0.0f, deltaYaw, 0.0f);

	m_scene.Update(*this, timer);
}
void Graphics::RenderUpdate(const Common::Timer& timer)
{
	UpdateCamera();
	UpdateLights(timer);
	UpdateMainPassData(timer);
	UpdateShadowPassData(timer);
	UpdateMaterialData();
	UpdateBillboards();

	//Common::PerformanceTimer performanceTimer;
	//performanceTimer.Start();
	UpdateInstancesDataFrustumCulling();
	//	UpdateInstancesDataOctreeCulling();
	//performanceTimer.End();
	//auto elapsedTime = performanceTimer.ElapsedTime<float, std::milli>().count();
	//auto string = L"ElapsedTime: " + std::to_wstring(elapsedTime) + L"\n";
	//OutputDebugStringW(string.c_str());

	if(m_cubeMapSkipFramesCurrentCount >= m_cubeMapSkipFramesCount)
		UpdateCubeMappingPassData(timer);
}

void Graphics::Render(const Common::Timer& timer)
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	m_d3dBase.BeginScene();

	if (m_debugWindowMode == DebugMode::Hidden || m_debugWindowMode == DebugMode::TerrainHeightMap || m_debugWindowMode == DebugMode::ShadowMap)
		DrawInNormalMode();
	else
		DrawInDebugMode();

	m_d3dBase.EndScene();
}

Camera* Graphics::GetCamera()
{
	return &m_camera;
}
DefaultScene* Graphics::GetScene()
{
	return &m_scene;
}

void Graphics::AddNormalRenderItem(std::unique_ptr<NormalRenderItem>&& renderItem, std::initializer_list<RenderLayer> renderLayers)
{
	for (auto renderLayer : renderLayers)
	{
		m_renderItemLayers[static_cast<SIZE_T>(renderLayer)].push_back(renderItem.get());
	}

	m_normalRenderItems.push_back(renderItem.get());

	for (auto& collider : renderItem->GetColliders())
		m_octree.AddObject(&collider);

	m_allRenderItems.push_back(std::move(renderItem));
}

void Graphics::AddNormalRenderItemInstance(NormalRenderItem* renderItem, const ShaderBufferTypes::InstanceData& instanceData) const
{
	renderItem->AddInstance(instanceData);

	if (m_initialized)
		m_currentFrameResource->RealocateInstanceBuffer(m_d3dBase.GetDevice(), renderItem);
}
void Graphics::AddBillboardRenderItem(std::unique_ptr<BillboardRenderItem>&& renderItem, std::initializer_list<RenderLayer> renderLayers)
{
	for (auto renderLayer : renderLayers)
		m_renderItemLayers[static_cast<SIZE_T>(renderLayer)].push_back(renderItem.get());

	m_billboardRenderItems.push_back(renderItem.get());
	m_allRenderItems.push_back(std::move(renderItem));
}
void Graphics::AddBillboardRenderItemInstance(BillboardRenderItem* renderItem, const BillboardMeshGeometry::VertexType& instanceData) const
{
	renderItem->AddInstance(m_d3dBase.GetDevice(), instanceData);
}
void Graphics::AddCubeMappingRenderItem(std::unique_ptr<CubeMappingRenderItem>&& renderItem, std::initializer_list<RenderLayer> renderLayers)
{
	for (auto renderLayer : renderLayers)
		m_renderItemLayers[static_cast<SIZE_T>(renderLayer)].push_back(renderItem.get());

	m_cubeMappingRenderItems.push_back(renderItem.get());
	m_allRenderItems.push_back(std::move(renderItem));
}

uint32_t Graphics::GetVisibleInstances() const
{
	return m_visibleInstances;
}
const std::vector<RenderItem*>& Graphics::GetRenderItems(RenderLayer renderLayer) const
{
	return m_renderItemLayers[static_cast<size_t>(renderLayer)];
}

std::vector<std::unique_ptr<RenderItem>>::const_iterator Graphics::GetRenderItem(const std::string& name) const
{
	auto match = [&name](const std::unique_ptr<RenderItem>& renderItem)
	{
		return renderItem->GetName() == name;
	};

	return std::find_if(m_allRenderItems.begin(), m_allRenderItems.end(), match);
}
std::vector<NormalRenderItem*>::const_iterator Graphics::GetNormalRenderItem(const std::string& name) const
{
	auto match = [&name](const NormalRenderItem* renderItem)
	{
		return renderItem->GetName() == name;
	};

	return std::find_if(m_normalRenderItems.begin(), m_normalRenderItems.end(), match);
}
std::vector<BillboardRenderItem*>::const_iterator Graphics::GetBillboardRenderItem(const std::string& name) const
{
	auto match = [&name](const BillboardRenderItem* renderItem)
	{
		return renderItem->GetName() == name;
	};

	return std::find_if(m_billboardRenderItems.begin(), m_billboardRenderItems.end(), match);
}
std::vector<CubeMappingRenderItem*>::const_iterator Graphics::GetCubeMappingRenderItem(const std::string& name) const
{
	auto match = [&name](const CubeMappingRenderItem* renderItem)
	{
		return renderItem->GetName() == name;
	};

	return std::find_if(m_cubeMappingRenderItems.begin(), m_cubeMappingRenderItems.end(), match);
}

void Graphics::SetFogState(bool state)
{
	m_fog = state;
}
void Graphics::SetFogDistanceParameters(float start, float range)
{
	m_mainPassData.FogStart = start;
	m_mainPassData.FogRange = range;

	if (m_mainPassData.FogColor.w == 1.0f)
		m_camera.SetFarZ(start + range);
}

void Graphics::SetFogColor(const DirectX::XMFLOAT4& color)
{
	m_mainPassData.FogColor = color;
	m_d3dBase.SetClearColor(XMFLOAT3(color.x, color.y, color.z));

	if (color.w < 1.0f)
		m_camera.SetFarZ(1024.0f);
}
Graphics::DebugMode Graphics::GetDebugWindowMode() const
{
	return m_debugWindowMode;
}
void Graphics::SetDebugWindowMode(DebugMode debugWindowMode)
{
	m_debugWindowMode = debugWindowMode;
}

void Graphics::ToggleDebugMode()
{
	m_debugWindowMode = static_cast<DebugMode>((static_cast<size_t>(m_debugWindowMode) + 1) % static_cast<size_t>(DebugMode::Count));
}

void Graphics::BindSamplers() const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	// Set samplers:
	deviceContext->DSSetSamplers(3, 1, m_linearClampSamplerState.GetAddressOf());
	deviceContext->PSSetSamplers(3, 1, m_linearClampSamplerState.GetAddressOf());
	deviceContext->DSSetSamplers(4, 1, m_anisotropicWrapSamplerState.GetAddressOf());
	deviceContext->PSSetSamplers(4, 1, m_anisotropicWrapSamplerState.GetAddressOf());
	deviceContext->DSSetSamplers(5, 1, m_anisotropicClampSamplerState.GetAddressOf());
	deviceContext->PSSetSamplers(5, 1, m_anisotropicClampSamplerState.GetAddressOf());
	deviceContext->PSSetSamplers(6, 1, m_shadowsSamplerState.GetAddressOf());
}

void Graphics::SetupTerrainMeshData()
{
	// Update main pass:
	{
		auto maxTesselationFactor = m_mainPassData.MaxTesselationFactor;
		auto minTesselationFactor = m_mainPassData.MinTesselationFactor;
		m_mainPassData.MaxTesselationFactor = 3.0f;
		m_mainPassData.MinTesselationFactor = 3.0f;
		UpdateMainPassData(Common::Timer(100.0f));
		m_mainPassData.MaxTesselationFactor = maxTesselationFactor;
		m_mainPassData.MinTesselationFactor = minTesselationFactor;
	}

	auto& terrain = m_scene.GetTerrain();
	const auto& terrainDescription = terrain.GetDescription();
	uint32_t vertexCount = (terrainDescription.CellXCount * terrainDescription.CellZCount) * 2 * 3 * 64;
	uint32_t stride = sizeof(VertexTypes::PositionVertexType);
	StreamOutputBuffer streamOutputBuffer(m_d3dBase.GetDevice(), vertexCount * stride, stride);

	auto deviceContext = m_d3dBase.GetDeviceContext();

	UINT offsets = 0;
	deviceContext->SOSetTargets(1, streamOutputBuffer.GetAddressOf(), &offsets);

	deviceContext->VSSetConstantBuffers(2, 1, m_currentFrameResource->MainPassData.GetAddressOf());
	deviceContext->HSSetConstantBuffers(2, 1, m_currentFrameResource->MainPassData.GetAddressOf());
	deviceContext->DSSetConstantBuffers(2, 1, m_currentFrameResource->MainPassData.GetAddressOf());
	deviceContext->GSSetConstantBuffers(2, 1, m_currentFrameResource->MainPassData.GetAddressOf());
	m_pipelineStateManager.SetPipelineState(deviceContext, "TerrainStreamOutput");
	DrawTerrain();

	StagingBuffer stagingBuffer(m_d3dBase.GetDevice(), streamOutputBuffer.GetSize(), streamOutputBuffer.GetStride());
	deviceContext->CopyResource(stagingBuffer.Get(), streamOutputBuffer.Get());

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	stagingBuffer.Map(deviceContext, D3D11_MAP_READ, &mappedSubresource);
	std::vector<VertexTypes::PositionVertexType> vertices(vertexCount);
	memcpy_s(vertices.data(), vertices.size() * sizeof(VertexTypes::PositionVertexType), mappedSubresource.pData, streamOutputBuffer.GetSize());
	stagingBuffer.Unmap(deviceContext);

	std::vector<uint32_t> indices(vertexCount);
	std::iota(indices.begin(), indices.end(), 0);

	terrain.SetMeshData(std::move(vertices), std::move(indices));
}
void Graphics::SetupDebugMode()
{
	m_debugPipelineStateNames.emplace(DebugMode::TerrainNormalVectors, "TerrainDebugNormalVectors");
	m_debugPipelineStateNames.emplace(DebugMode::TerrainTangentVectors, "TerrainDebugTangentVectors");
	m_debugPipelineStateNames.emplace(DebugMode::TerrainPathAlpha, "TerrainDebugPathAlpha");
	m_debugPipelineStateNames.emplace(DebugMode::TerrainNormalMapping, "TerrainDebugNormalMapping");
	m_debugPipelineStateNames.emplace(DebugMode::TerrainSpecularMapping, "TerrainDebugSpecularMapping");
	m_debugPipelineStateNames.emplace(DebugMode::TerrainWireframe, "TerrainDebugWireframe");
	m_debugPipelineStateNames.emplace(DebugMode::TerrainHeightMap, "DebugWindowHeightMap");
	m_debugPipelineStateNames.emplace(DebugMode::ShadowMap, "DebugWindowSingleChannel");
}
void Graphics::UpdateCamera()
{
	m_camera.Update();
}
void Graphics::UpdateInstancesDataFrustumCulling()
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	// Get view matrix and calculate its inverse:
	auto viewMatrix = m_camera.GetViewMatrix();
	auto viewMatrixDeterminant = XMMatrixDeterminant(viewMatrix);
	auto inverseViewMatrix = XMMatrixInverse(&viewMatrixDeterminant, viewMatrix);

	// Build the view space camera frustum:
	auto viewSpaceCameraFrustum = m_camera.BuildViewSpaceBoundingFrustum();

	m_visibleInstances = 0;
	for (const auto& renderItem : m_normalRenderItems)
	{
		// Get instances buffer for the current render item:
		auto location = m_currentFrameResource->InstancesBuffers.find(renderItem->GetName());
		if (location == m_currentFrameResource->InstancesBuffers.end())
			continue;

		const auto& instancesBuffer = location->second;

		if (renderItem->GetInstancesData().size() == 0)
			continue;

		// Map resource:
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		instancesBuffer.Map(deviceContext, D3D11_MAP_WRITE_DISCARD, &mappedResource);
		auto instacesBufferView = reinterpret_cast<ShaderBufferTypes::InstanceData*>(mappedResource.pData);

		// For each instance:
		auto visibleInstanceCount = 0;
		for (const auto& instanceData : renderItem->GetInstancesData())
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
			if (localSpaceCameraFrustum.Contains(renderItem->GetSubmesh().Bounds) != ContainmentType::DISJOINT)
			{
				// Update instance data:
				auto& instanceDataView = instacesBufferView[visibleInstanceCount++];
				instanceDataView.WorldMatrix = instanceData.WorldMatrix;
			}
		}
		renderItem->SetVisibleInstanceCount(visibleInstanceCount);
		m_visibleInstances += visibleInstanceCount;

		// Unmap resource:
		instancesBuffer.Unmap(deviceContext);
	}

	// Cube map:
	{
		auto cubeMapRenderItem = m_cubeMappingRenderItems[0];
		const auto& instanceData = cubeMapRenderItem->GetInstanceData();

		auto position = cubeMapRenderItem->GetPosition();
		auto distance = XMVector3Length(XMLoadFloat3(&position) - m_camera.GetPosition());
		if (XMVectorGetX(distance) > 15.0f)
		{
			m_cubeMapSkipFramesCount = 120;
			return;
		}

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
		if (localSpaceCameraFrustum.Contains(cubeMapRenderItem->GetSubmesh().Bounds) != ContainmentType::DISJOINT)
		{
			m_cubeMapSkipFramesCount = 4;
		}
		else
		{
			m_cubeMapSkipFramesCount = 120;
		}
	}
}
void Graphics::UpdateInstancesDataOctreeCulling()
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	// Get view matrix and calculate its inverse:
	auto viewMatrix = m_camera.GetViewMatrix();
	auto viewMatrixDeterminant = XMMatrixDeterminant(viewMatrix);
	auto inverseViewMatrix = XMMatrixInverse(&viewMatrixDeterminant, viewMatrix);

	// Build the view space camera frustum:
	auto viewSpaceCameraFrustum = m_camera.BuildViewSpaceBoundingFrustum();

	m_octree.CalculateIntersections(viewSpaceCameraFrustum, inverseViewMatrix);

	m_visibleInstances = 0;
	for (const auto& renderItem : m_normalRenderItems)
	{
		// Get instances buffer for the current render item:
		auto location = m_currentFrameResource->InstancesBuffers.find(renderItem->GetName());
		if (location == m_currentFrameResource->InstancesBuffers.end())
			continue;

		const auto& instancesBuffer = location->second;

		const auto& visibleInstances = renderItem->GetVisibleInstances();
		if (visibleInstances.empty())
			continue;

		// Map resource:
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		instancesBuffer.Map(deviceContext, D3D11_MAP_WRITE_DISCARD, &mappedResource);
		auto instacesBufferView = reinterpret_cast<ShaderBufferTypes::InstanceData*>(mappedResource.pData);

		// Update instance data:
		auto visibleInstanceCount = 0;
		for (auto instanceID : visibleInstances)
		{
			auto& instanceDataView = instacesBufferView[visibleInstanceCount++];
			const auto& instanceData = renderItem->GetInstance(instanceID);
			instanceDataView.WorldMatrix = instanceData.WorldMatrix;
		}
		renderItem->ClearVisibleInstances();

		m_visibleInstances += visibleInstanceCount;
		renderItem->SetVisibleInstanceCount(visibleInstanceCount);

		// Unmap resource:
		instancesBuffer.Unmap(deviceContext);
	}
}
void Graphics::UpdateBillboards()
{
	auto deviceContext = m_d3dBase.GetDeviceContext();
	for (auto& renderItem : m_billboardRenderItems)
		renderItem->Update(deviceContext);
}
void Graphics::UpdateMaterialData() const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	for (auto& e : m_scene.GetMaterials())
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
void Graphics::UpdateLights(const Common::Timer& timer) const
{
	const auto& castShadowsLights = m_lightManager.GetCastShadowsLights();
	auto castShadowsLight = castShadowsLights[0];

	// Update light matrices:
	castShadowsLight->UpdateMatrices(m_sceneBounds);
}

void Graphics::InitializeMainPassData()
{
	m_mainPassData.TerrainDisplacementScalarY = 1.0f;
	m_mainPassData.FogStart = 20.0f;
	m_mainPassData.FogRange = 100.0f;
	m_mainPassData.MaxTesselationDistance = 100.0f;
	m_mainPassData.MaxTesselationFactor = 5.0f;
	m_mainPassData.MinTesselationDistance = 1000.0f;
	m_mainPassData.MinTesselationFactor = 0.0f;
	m_mainPassData.SkyDomeColors[0] = { 0.9f, 0.9f, 0.97f, 1.0f };
	m_mainPassData.SkyDomeColors[1] = { 0.17f, 0.30f, 0.51f, 1.0f };
}
void Graphics::UpdateMainPassData(const Common::Timer& timer)
{
	auto viewMatrix = m_camera.GetViewMatrix();
	auto viewMatrixDeterminant = XMMatrixDeterminant(viewMatrix);
	auto inverseViewMatrix = XMMatrixInverse(&viewMatrixDeterminant, viewMatrix);

	auto projectionMatrix = m_camera.GetProjectionMatrix();
	auto projectionMatrixDeterminant = XMMatrixDeterminant(projectionMatrix);
	auto inverseProjectionMatrix = XMMatrixInverse(&projectionMatrixDeterminant, projectionMatrix);

	auto viewProjectionMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);
	auto viewProjectionMatrixDeterminant = XMMatrixDeterminant(viewProjectionMatrix);
	auto inverseViewProjectionMatrix = XMMatrixInverse(&viewProjectionMatrixDeterminant, viewProjectionMatrix);

	XMStoreFloat4x4(&m_mainPassData.ViewMatrix, XMMatrixTranspose(viewMatrix));
	XMStoreFloat4x4(&m_mainPassData.InverseViewMatrix, XMMatrixTranspose(inverseViewMatrix));
	XMStoreFloat4x4(&m_mainPassData.ProjectionMatrix, XMMatrixTranspose(projectionMatrix));
	XMStoreFloat4x4(&m_mainPassData.InverseProjectionMatrix, XMMatrixTranspose(inverseProjectionMatrix));
	XMStoreFloat4x4(&m_mainPassData.ViewProjectionMatrix, XMMatrixTranspose(viewProjectionMatrix));
	XMStoreFloat4x4(&m_mainPassData.InverseProjectionMatrix, XMMatrixTranspose(inverseViewProjectionMatrix));

	const auto& castShadowsLights = m_lightManager.GetCastShadowsLights();
	auto castShadowsLight = castShadowsLights[0];
	XMStoreFloat4x4(&m_mainPassData.ShadowMatrix, XMMatrixTranspose(castShadowsLight->GetShadowMatrix()));

	const auto& grassTransformMatrix = m_scene.GetGrassTransformMatrix();
	XMStoreFloat4x4(&m_mainPassData.GrassTransformMatrix, XMLoadFloat4x4(&grassTransformMatrix));

	XMStoreFloat4x4(&m_mainPassData.SkyCloudsTransformMatrix, XMMatrixRotationX(-XM_PI / 2.0f));

	XMStoreFloat3(&m_mainPassData.EyePositionW, m_camera.GetPosition());
	m_mainPassData.RenderTargetSize = XMFLOAT2(static_cast<float>(m_d3dBase.GetClientWidth()), static_cast<float>(m_d3dBase.GetClientHeight()));
	m_mainPassData.InverseRenderTargetSize = XMFLOAT2(1.0f / static_cast<float>(m_d3dBase.GetClientWidth()), 1.0f / static_cast<float>(m_d3dBase.GetClientHeight()));
	m_mainPassData.NearZ = m_camera.GetNearZ();
	m_mainPassData.FarZ = m_camera.GetFarZ();
	m_mainPassData.TotalTime = static_cast<float>(timer.GetTotalMilliseconds());
	m_mainPassData.DeltaTime = static_cast<float>(timer.GetDeltaMilliseconds());

	const auto& terrain = m_scene.GetTerrain();
	m_mainPassData.TexelSize = terrain.GetTexelSize();
	m_mainPassData.TiledTexelScale = terrain.GetDescription().TiledTexelScale;

	m_mainPassData.AmbientLight = m_lightManager.GetAmbientLight();
	m_lightManager.Fill(m_mainPassData.Lights.begin(), m_mainPassData.Lights.end());

	m_currentFrameResource->MainPassData.CopyData(m_d3dBase.GetDeviceContext(), &m_mainPassData, sizeof(ShaderBufferTypes::PassData));
}
void Graphics::UpdateShadowPassData(const Common::Timer& timer) const
{
	ShaderBufferTypes::PassData passData = m_mainPassData;

	auto castShadowsLights = m_lightManager.GetCastShadowsLights();
	auto castShadowsLight = castShadowsLights[0];

	auto viewMatrix = castShadowsLight->GetViewMatrix();
	auto viewMatrixDeterminant = XMMatrixDeterminant(viewMatrix);
	auto inverseViewMatrix = XMMatrixInverse(&viewMatrixDeterminant, viewMatrix);

	auto projectionMatrix = castShadowsLight->GetProjectionMatrix();
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

	const auto& grassTransformMatrix = m_scene.GetGrassTransformMatrix();
	XMStoreFloat4x4(&passData.GrassTransformMatrix, XMLoadFloat4x4(&grassTransformMatrix));

	XMStoreFloat3(&passData.EyePositionW, m_camera.GetPosition());
	passData.TerrainDisplacementScalarY = 1.0f;
	passData.RenderTargetSize = XMFLOAT2(static_cast<float>(m_d3dBase.GetClientWidth()), static_cast<float>(m_d3dBase.GetClientHeight()));
	passData.InverseRenderTargetSize = XMFLOAT2(1.0f / static_cast<float>(m_d3dBase.GetClientWidth()), 1.0f / static_cast<float>(m_d3dBase.GetClientHeight()));
	passData.NearZ = m_camera.GetNearZ();
	passData.FarZ = m_camera.GetFarZ();
	passData.TotalTime = static_cast<float>(timer.GetTotalMilliseconds());
	passData.DeltaTime = static_cast<float>(timer.GetDeltaMilliseconds());

	const auto& terrain = m_scene.GetTerrain();
	passData.TexelSize = terrain.GetTexelSize();
	passData.TiledTexelScale = terrain.GetDescription().TiledTexelScale;

	passData.SkyDomeColors[0] = { 0.5f, 0.1f, 0.1f, 1.0f };
	passData.SkyDomeColors[1] = { 0.1f, 0.1f, 0.8f, 1.0f };
	passData.AmbientLight = m_lightManager.GetAmbientLight();
	m_lightManager.Fill(passData.Lights.begin(), passData.Lights.end());

	m_currentFrameResource->ShadowPassData.CopyData(m_d3dBase.GetDeviceContext(), &passData, sizeof(ShaderBufferTypes::PassData));
}
void Graphics::UpdateCubeMappingPassData(const Common::Timer& timer) const
{
	auto passData = m_mainPassData;

	const auto& camera = m_cubeMappingRenderItems[0]->GetCamera();
	for (size_t i = 0; i < 6; ++i)
	{
		auto viewMatrix = camera.GetViewMatrix(i);
		auto viewMatrixDeterminant = XMMatrixDeterminant(viewMatrix);
		auto inverseViewMatrix = XMMatrixInverse(&viewMatrixDeterminant, viewMatrix);

		auto projectionMatrix = camera.GetProjectionMatrix();
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

		XMStoreFloat3(&passData.EyePositionW, camera.GetPosition());
		passData.NearZ = camera.GetNearZ();
		passData.FarZ = camera.GetFarZ();

		m_currentFrameResource->CubeMapPassData[i].CopyData(m_d3dBase.GetDeviceContext(), &passData, sizeof(ShaderBufferTypes::PassData));
	}
}

void Graphics::SetPassData(ID3D11Buffer* const* ppPassDataBuffer) const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();
	deviceContext->VSSetConstantBuffers(2, 1, ppPassDataBuffer);
	deviceContext->HSSetConstantBuffers(2, 1, ppPassDataBuffer);
	deviceContext->DSSetConstantBuffers(2, 1, ppPassDataBuffer);
	deviceContext->GSSetConstantBuffers(2, 1, ppPassDataBuffer);
	deviceContext->PSSetConstantBuffers(2, 1, ppPassDataBuffer);
}

void Graphics::DrawInNormalMode()
{
	auto deviceContext = m_d3dBase.GetDeviceContext();
	std::array<ID3D11ShaderResourceView*, 1> nullSRV = { nullptr };

	// Create shadow map:
	if (m_enableShadows)
	{
		DrawSceneIntoShadowMap(m_shadowMap);
	}

	// Draw scene into cube map:
	if (m_cubeMapSkipFramesCurrentCount >= m_cubeMapSkipFramesCount)
	{
		m_cubeMapSkipFramesCurrentCount = 0;

		// Unbind cube map shader resource view:
		deviceContext->PSSetShaderResources(15, static_cast<UINT>(nullSRV.size()), nullSRV.data());

		// Draw scene into cube map:
		const auto& cubeMapRenderItem = m_cubeMappingRenderItems[0];
		const auto& cubeMapRenderTexture = cubeMapRenderItem->GetRenderTexture();
		DrawSceneIntoCubeMap(deviceContext, cubeMapRenderTexture);

		// Set default render target and depth stencil:
		m_d3dBase.SetDefaultRenderTargets();

		// Set cube map as shader resource view:
		std::array<ID3D11ShaderResourceView*, 1> cubeMap = { cubeMapRenderTexture.GetShaderResourceView() };
		deviceContext->PSSetShaderResources(15, static_cast<UINT>(cubeMap.size()), cubeMap.data());
	}
	else
	{
		++m_cubeMapSkipFramesCurrentCount;
	}

	// Set default render target and depth stencil:
	m_d3dBase.SetDefaultRenderTargets();

	// Set main pass data:
	SetPassData(m_currentFrameResource->MainPassData.GetAddressOf());

	// Draw main scene:
	DrawMainScene(true);

	// Draw debug window:
	DrawDebugWindow();

	deviceContext->PSSetShaderResources(3, static_cast<UINT>(nullSRV.size()), nullSRV.data());
	deviceContext->PSSetShaderResources(0, static_cast<UINT>(nullSRV.size()), nullSRV.data());
	m_shadowMap.ClearDepthStencilView(deviceContext);
}

void Graphics::DrawRenderItems(RenderLayer renderLayer) const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	UINT stride = sizeof(ShaderBufferTypes::InstanceData);
	UINT offset = 0;

	// For each render item:
	for (auto& renderItem : m_renderItemLayers[static_cast<SIZE_T>(renderLayer)])
	{
		// Set instances data:
		deviceContext->IASetVertexBuffers(1, 1, m_currentFrameResource->InstancesBuffers[renderItem->GetName()].GetAddressOf(), &stride, &offset);

		// Set material data:
		auto pMaterial = renderItem->GetMaterial();
		const auto& materialData = m_currentFrameResource->MaterialDataArray[pMaterial->MaterialIndex];
		deviceContext->VSSetConstantBuffers(1, 1, materialData.GetAddressOf());
		deviceContext->GSSetConstantBuffers(1, 1, materialData.GetAddressOf());
		deviceContext->PSSetConstantBuffers(1, 1, materialData.GetAddressOf());

		// Set textures:
		if (pMaterial->DiffuseMap != nullptr)
			deviceContext->PSSetShaderResources(0, 1, pMaterial->DiffuseMap->GetAddressOf());
		if (pMaterial->NormalMap != nullptr)
			deviceContext->PSSetShaderResources(1, 1, pMaterial->NormalMap->GetAddressOf());
		if (pMaterial->SpecularMap != nullptr)
			deviceContext->PSSetShaderResources(2, 1, pMaterial->SpecularMap->GetAddressOf());

		// Render:
		renderItem->Render(deviceContext);
	}
}
void Graphics::DrawNonInstancedRenderItems(RenderLayer renderLayer) const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	// For each render item:
	for (auto& renderItem : m_renderItemLayers[static_cast<SIZE_T>(renderLayer)])
	{
		// Set material data:
		auto pMaterial = renderItem->GetMaterial();
		const auto& materialData = m_currentFrameResource->MaterialDataArray[pMaterial->MaterialIndex];
		deviceContext->VSSetConstantBuffers(1, 1, materialData.GetAddressOf());
		deviceContext->GSSetConstantBuffers(1, 1, materialData.GetAddressOf());
		deviceContext->PSSetConstantBuffers(1, 1, materialData.GetAddressOf());

		// Set textures:
		if (pMaterial->DiffuseMap != nullptr)
			deviceContext->PSSetShaderResources(0, 1, pMaterial->DiffuseMap->GetAddressOf());
		if (pMaterial->NormalMap != nullptr)
			deviceContext->PSSetShaderResources(1, 1, pMaterial->NormalMap->GetAddressOf());
		if (pMaterial->SpecularMap != nullptr)
			deviceContext->PSSetShaderResources(2, 1, pMaterial->SpecularMap->GetAddressOf());

		// Render:
		renderItem->RenderNonInstanced(deviceContext);
	}
}
void Graphics::DrawTerrain() const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	// For each render item:
	for (auto& renderItem : m_renderItemLayers[static_cast<SIZE_T>(RenderLayer::Terrain)])
	{
		// Set material data:
		auto pMaterial = renderItem->GetMaterial();
		const auto& materialData = m_currentFrameResource->MaterialDataArray[pMaterial->MaterialIndex];
		deviceContext->VSSetConstantBuffers(1, 1, materialData.GetAddressOf());
		deviceContext->PSSetConstantBuffers(1, 1, materialData.GetAddressOf());

		// Set textures:
		deviceContext->PSSetShaderResources(0, 1, pMaterial->NormalMap->GetAddressOf());
		deviceContext->DSSetShaderResources(1, 1, pMaterial->HeightMap->GetAddressOf());
		deviceContext->PSSetShaderResources(1, 1, pMaterial->HeightMap->GetAddressOf());
		deviceContext->PSSetShaderResources(2, 1, pMaterial->TangentMap->GetAddressOf());

		UINT startSlot = 4;

		for (auto tiledMapsArray : pMaterial->TiledMapsArrays)
		{
			auto numViews = static_cast<UINT>(tiledMapsArray.GetSize());
			deviceContext->PSSetShaderResources(startSlot, numViews, tiledMapsArray.GetTextureArray());
			startSlot += numViews;
		}

		// Render:
		renderItem->RenderNonInstanced(deviceContext);
	}
}
void Graphics::DrawInDebugMode() const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	m_pipelineStateManager.SetPipelineState(deviceContext, m_debugPipelineStateNames.at(m_debugWindowMode));
	DrawTerrain();
}
void Graphics::DrawSceneIntoShadowMap(const ShadowTexture& shadowMap) const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	// Set shadow pass data:
	deviceContext->VSSetConstantBuffers(2, 1, m_currentFrameResource->ShadowPassData.GetAddressOf());
	deviceContext->HSSetConstantBuffers(2, 1, m_currentFrameResource->ShadowPassData.GetAddressOf());
	deviceContext->DSSetConstantBuffers(2, 1, m_currentFrameResource->ShadowPassData.GetAddressOf());
	deviceContext->GSSetConstantBuffers(2, 1, m_currentFrameResource->ShadowPassData.GetAddressOf());
	deviceContext->PSSetConstantBuffers(2, 1, m_currentFrameResource->ShadowPassData.GetAddressOf());

	// Bind a depth stencil view to record the depth of the scene from the light point of view:
	shadowMap.SetDepthStencilView(deviceContext);

	if (!m_drawTerrainOnly)
	{
		// Draw opaque:
		m_pipelineStateManager.SetPipelineState(deviceContext, "OpaqueShadow");
		DrawRenderItems(RenderLayer::Opaque);
		DrawRenderItems(RenderLayer::NormalSpecularMapping);
		DrawRenderItems(RenderLayer::OpaqueDynamicReflectors);
	}

	// Draw terrain:
	m_pipelineStateManager.SetPipelineState(deviceContext, "TerrainShadow");
	DrawTerrain();

	if (!m_drawTerrainOnly)
	{
		// Draw transparent:
		m_pipelineStateManager.SetPipelineState(deviceContext, "TransparentShadow");
		DrawRenderItems(RenderLayer::Transparent);

		// Draw alpha-clipped:
		m_pipelineStateManager.SetPipelineState(deviceContext, "AlphaClippedShadow");
		DrawRenderItems(RenderLayer::AlphaClipped);
	}
}
void Graphics::DrawSceneIntoCubeMap(ID3D11DeviceContext* deviceContext, const CubeMapRenderTexture& cubeMap) const
{
	cubeMap.SetViewport(deviceContext);
	for (size_t i = 0; i < 6; ++i)
	{
		cubeMap.ClearRenderTarget(deviceContext, static_cast<UINT>(i));
		cubeMap.SetRenderTarget(deviceContext, static_cast<UINT>(i));
		SetPassData(m_currentFrameResource->CubeMapPassData[i].GetAddressOf());
		DrawMainScene(false);
	}
	m_d3dBase.SetViewport();

	deviceContext->GenerateMips(cubeMap.GetShaderResourceView());
}
void Graphics::DrawMainScene(bool drawCubeMapRenderItems) const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	// Bind shadow map for use in shaders:
	auto shadowMapSRV = m_shadowMap.GetShaderResourceView();
	deviceContext->PSSetShaderResources(3, 1, &shadowMapSRV);

	if (!m_fog)
	{
		// Draw Skydome:
		m_pipelineStateManager.SetPipelineState(deviceContext, "SkyDome");
		DrawNonInstancedRenderItems(RenderLayer::SkyDome);

		// Draw Sky Clouds:
		m_pipelineStateManager.SetPipelineState(deviceContext, "SkyCloudsFog");
		DrawNonInstancedRenderItems(RenderLayer::SkyClouds);

		if (!m_drawTerrainOnly)
		{
			// Draw opaque:
			m_pipelineStateManager.SetPipelineState(deviceContext, "Opaque");
			DrawRenderItems(RenderLayer::Opaque);

			// Draw normal mapped:
			m_pipelineStateManager.SetPipelineState(deviceContext, "NormalSpecularMapping");
			DrawRenderItems(RenderLayer::NormalSpecularMapping);

			// Draw cube mapped:
			if (drawCubeMapRenderItems)
			{
				m_pipelineStateManager.SetPipelineState(deviceContext, "StandardCubeMapping");
				DrawRenderItems(RenderLayer::OpaqueDynamicReflectors);
			}
		}

		// Draw terrain:
		m_pipelineStateManager.SetPipelineState(deviceContext, "Terrain");
		DrawTerrain();

		if (!m_drawTerrainOnly)
		{
			// Draw transparent:
			m_pipelineStateManager.SetPipelineState(deviceContext, "Transparent");
			DrawRenderItems(RenderLayer::Transparent);

			// Draw alpha-clipped:
			m_pipelineStateManager.SetPipelineState(deviceContext, "AlphaClipped");
			DrawRenderItems(RenderLayer::AlphaClipped);

			// Draw billboards:
			m_pipelineStateManager.SetPipelineState(deviceContext, "Billboard");
			DrawNonInstancedRenderItems(RenderLayer::Grass);
		}
	}
	else
	{
		// Draw Skydome:
		m_pipelineStateManager.SetPipelineState(deviceContext, "SkyDomeFog");
		DrawNonInstancedRenderItems(RenderLayer::SkyDome);

		// Draw Sky Clouds:
		m_pipelineStateManager.SetPipelineState(deviceContext, "SkyCloudsFog");
		DrawNonInstancedRenderItems(RenderLayer::SkyClouds);

		if (!m_drawTerrainOnly)
		{
			// Draw opaque:
			m_pipelineStateManager.SetPipelineState(deviceContext, "OpaqueFog");
			DrawRenderItems(RenderLayer::Opaque);

			// Draw normal mapped:
			m_pipelineStateManager.SetPipelineState(deviceContext, "NormalSpecularMappingFog");
			DrawRenderItems(RenderLayer::NormalSpecularMapping);

			// Draw cube mapped:
			if (drawCubeMapRenderItems)
			{
				m_pipelineStateManager.SetPipelineState(deviceContext, "StandardCubeMappingFog");
				DrawRenderItems(RenderLayer::OpaqueDynamicReflectors);
			}
		}

		// Draw terrain:
		m_pipelineStateManager.SetPipelineState(deviceContext, "TerrainFog");
		DrawTerrain();

		if (!m_drawTerrainOnly)
		{
			// Draw transparent:
			m_pipelineStateManager.SetPipelineState(deviceContext, "TransparentFog");
			DrawRenderItems(RenderLayer::Transparent);

			// Draw alpha-clipped:
			m_pipelineStateManager.SetPipelineState(deviceContext, "AlphaClippedFog");
			DrawRenderItems(RenderLayer::AlphaClipped);

			// Draw billboards:
			m_pipelineStateManager.SetPipelineState(deviceContext, "BillboardFog");
			DrawNonInstancedRenderItems(RenderLayer::Grass);
		}
	}
}

void Graphics::DrawDebugWindow() const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	if (m_debugWindowMode == DebugMode::ShadowMap || m_debugWindowMode == DebugMode::TerrainHeightMap)
	{
		m_pipelineStateManager.SetPipelineState(deviceContext, m_debugPipelineStateNames.at(m_debugWindowMode));

		if (m_debugWindowMode == DebugMode::ShadowMap)
		{
			auto shadowMapSRV = m_shadowMap.GetShaderResourceView();
			deviceContext->PSSetShaderResources(0, 1, &shadowMapSRV);
		}
		else if (m_debugWindowMode == DebugMode::TerrainHeightMap)
			deviceContext->PSSetShaderResources(0, 1, m_renderItemLayers[static_cast<UINT>(RenderLayer::Terrain)][0]->GetMaterial()->HeightMap->GetAddressOf());

		DrawNonInstancedRenderItems(RenderLayer::Debug);
	}
}

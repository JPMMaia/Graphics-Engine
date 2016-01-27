#include "stdafx.h"
#include "TerrainEffect.h"
#include "SamplerStateDescConstants.h"
#include "RasterizerStateDescConstants.h"

using namespace GraphicsEngine;

TerrainEffect::TerrainEffect()
{
}
TerrainEffect::TerrainEffect(ID3D11Device* d3dDevice)
{
	Initialize(d3dDevice);
}

void TerrainEffect::Initialize(ID3D11Device* d3dDevice)
{
	// Describe the input layout:
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> inputDesc =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Initialize shaders:
	m_vertexShader.Initialize(d3dDevice, L"TerrainVertexShader.cso", inputDesc);
	m_hullShader.Initialize(d3dDevice, L"TerrainHullShader.cso");
	m_domainShader.Initialize(d3dDevice, L"TerrainDomainShader.cso");
	m_pixelShader.Initialize(d3dDevice, L"TerrainPixelShader.cso");

	// Initialize constant buffers:
	m_cameraConstantBuffer.Initialize<CameraConstantBuffer>(d3dDevice, sizeof(CameraConstantBuffer));
	m_tesselationConstantBuffer.Initialize<TesselationConstantBuffer>(d3dDevice, sizeof(TesselationConstantBuffer));
	m_subsetConstantBuffer.Initialize<SubsetConstantBuffer>(d3dDevice, sizeof(SubsetConstantBuffer));
	m_frameConstantBuffer.Initialize<FrameConstantBuffer>(d3dDevice, sizeof(FrameConstantBuffer));

	// Initialize the sampler state:
	m_samplerState.Initialize(d3dDevice, SamplerStateDescConstants::Anisotropic);

	// Initialize the rasterizer state:
	m_rasterizerState.Initialize(d3dDevice, RasterizerStateDescConstants::Default);

	// Setup light technique:
	m_terrainTechnique.SetVertexShader(&m_vertexShader);
	m_terrainTechnique.SetHullShader(&m_hullShader);
	m_terrainTechnique.SetDomainShader(&m_domainShader);
	m_terrainTechnique.SetPixelShader(&m_pixelShader);
	m_terrainTechnique.HSSetConstantBuffer(m_cameraConstantBuffer.Get(), 0);
	m_terrainTechnique.HSSetConstantBuffer(m_tesselationConstantBuffer.Get(), 1);
	m_terrainTechnique.DSSetConstantBuffer(m_cameraConstantBuffer.Get(), 0);
	m_terrainTechnique.DSSetSamplerState(m_samplerState, 0);

	m_terrainTechnique.SetRasterizerState(&m_rasterizerState);
}
void TerrainEffect::Reset()
{
	m_samplerState.Reset();
	m_frameConstantBuffer.Reset();
	m_subsetConstantBuffer.Reset();
	m_tesselationConstantBuffer.Reset();
	m_cameraConstantBuffer.Reset();
	m_pixelShader.Reset();
	m_domainShader.Reset();
	m_hullShader.Reset();
	m_vertexShader.Reset();
}

void TerrainEffect::UpdateCameraConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const CameraConstantBuffer& buffer) const
{
	m_cameraConstantBuffer.Map(d3dDeviceContext, &buffer, sizeof(CameraConstantBuffer));
}
void TerrainEffect::UpdateTesselationConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const TesselationConstantBuffer& buffer) const
{
	m_tesselationConstantBuffer.Map(d3dDeviceContext, &buffer, sizeof(TesselationConstantBuffer));
}
void TerrainEffect::UpdateSubsetConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const SubsetConstantBuffer& buffer) const
{
	m_subsetConstantBuffer.Map(d3dDeviceContext, &buffer, sizeof(SubsetConstantBuffer));
}
void TerrainEffect::UpdateFrameConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const FrameConstantBuffer& buffer) const
{
	m_frameConstantBuffer.Map(d3dDeviceContext, &buffer, sizeof(FrameConstantBuffer));
}

void TerrainEffect::Set(ID3D11DeviceContext1* d3dDeviceContext) const
{
	// Set technique:
	m_terrainTechnique.Set(d3dDeviceContext);
}

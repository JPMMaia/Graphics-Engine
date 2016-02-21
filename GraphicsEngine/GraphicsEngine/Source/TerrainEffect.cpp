#include "stdafx.h"
#include "TerrainEffect.h"
#include "SamplerStateDescConstants.h"
#include "RasterizerStateDescConstants.h"

#include <array>

using namespace GraphicsEngine;
using namespace std;

const array<D3D11_INPUT_ELEMENT_DESC, 2> TerrainEffect::s_INPUT_ELEMENT_DESCRIPTION =
{
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	}
};

TerrainEffect::TerrainEffect()
{
}
TerrainEffect::TerrainEffect(ID3D11Device* d3dDevice) :
	m_vertexShader(d3dDevice, L"TerrainVertexShader.cso", s_INPUT_ELEMENT_DESCRIPTION),
	m_hullShader(d3dDevice, L"TerrainHullShader.cso"),
	m_domainShader(d3dDevice, L"TerrainDomainShader.cso"),
	m_pixelShader(d3dDevice, L"TerrainPixelShader.cso"),
	m_cameraBuffer(d3dDevice, sizeof(CameraBuffer)),
	m_frameBuffer(d3dDevice, sizeof(FrameBuffer)),
	m_subsetBuffer(d3dDevice, sizeof(SubsetBuffer)),
	m_tesselationBuffer(d3dDevice, sizeof(TesselationBuffer)),
	m_samplerState(d3dDevice, SamplerStateDescConstants::Anisotropic),
	m_rasterizerState(d3dDevice, RasterizerStateDescConstants::Default)
{
}

void TerrainEffect::Reset()
{
	m_rasterizerState.Reset();
	m_samplerState.Reset();
	m_tesselationBuffer.Reset();
	m_subsetBuffer.Reset();
	m_frameBuffer.Reset();
	m_cameraBuffer.Reset();
	m_pixelShader.Reset();
	m_domainShader.Reset();
	m_hullShader.Reset();
	m_vertexShader.Reset();
}

void TerrainEffect::Set(ID3D11DeviceContext1* d3dDeviceContext) const
{
	// Set vertex shader:
	{
		m_vertexShader.Set(d3dDeviceContext);
		d3dDeviceContext->VSSetSamplers(0, 1, m_samplerState.GetAddressOf());
	}

	// Set hull shader:
	{
		m_hullShader.Set(d3dDeviceContext);
		static const array<ID3D11Buffer*, 2> constantBuffers = { m_cameraBuffer.Get(), m_tesselationBuffer.Get() };
		d3dDeviceContext->HSSetConstantBuffers(0, constantBuffers.size(), constantBuffers.data());
	}

	// Set domain shader:
	{
		m_domainShader.Set(d3dDeviceContext);
		d3dDeviceContext->DSSetConstantBuffers(0, 1, m_cameraBuffer.GetAddressOf());
		d3dDeviceContext->DSSetSamplers(0, 1, m_samplerState.GetAddressOf());
	}

	// Set pixel shader:
	{
		m_pixelShader.Set(d3dDeviceContext);
	}

	// Set rasterizer state:
	d3dDeviceContext->RSSetState(m_rasterizerState.Get());
}

void TerrainEffect::UpdateCameraBuffer(ID3D11DeviceContext1* d3dDeviceContext, const CameraBuffer& buffer) const
{
	m_cameraBuffer.Map(d3dDeviceContext, &buffer, sizeof(CameraBuffer));
}
void TerrainEffect::UpdateFrameBuffer(ID3D11DeviceContext1* d3dDeviceContext, const FrameBuffer& buffer) const
{
	m_frameBuffer.Map(d3dDeviceContext, &buffer, sizeof(FrameBuffer));
}
void TerrainEffect::UpdateSubsetBuffer(ID3D11DeviceContext1* d3dDeviceContext, const SubsetBuffer& buffer) const
{
	m_subsetBuffer.Map(d3dDeviceContext, &buffer, sizeof(SubsetBuffer));
}
void TerrainEffect::UpdateTesselationBuffer(ID3D11DeviceContext1* d3dDeviceContext, const TesselationBuffer& buffer) const
{
	m_tesselationBuffer.Map(d3dDeviceContext, &buffer, sizeof(TesselationBuffer));
}

void TerrainEffect::SetHeightMap(ID3D11DeviceContext1* d3dDeviceContext, const Texture& heightMap)
{
	d3dDeviceContext->VSSetShaderResources(0, 1, heightMap.GetAddressOf());
	d3dDeviceContext->DSSetShaderResources(0, 1, heightMap.GetAddressOf());
}
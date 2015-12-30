#include "stdafx.h"
#include "LightEffect.h"

#include "SamplerStateDescConstants.h"

using namespace GraphicsEngine;
using namespace std;

LightEffect::LightEffect()
{
}

LightEffect::LightEffect(ID3D11Device* d3dDevice)
{
	Initialize(d3dDevice);
}

void LightEffect::Initialize(ID3D11Device* d3dDevice)
{
	// Describe the input layout:
	static const vector<D3D11_INPUT_ELEMENT_DESC> inputDesc = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// Initialize the vertex shader:
	m_vertexShader.Initialize(d3dDevice, L"LightVertexShader.cso", inputDesc);

	// Initialize the pixel shader:
	m_pixelShader.Initialize(d3dDevice, L"LightPixelShader.cso");

	// Initialize constant buffers:
	m_perObjectConstantBuffer.Initialize(d3dDevice, sizeof(PerObjectConstantBuffer));
	m_perFrameConstantBuffer.Initialize(d3dDevice, sizeof(PerFrameConstantBuffer));

	// Initialize the sampler state:
	m_samplerState.Initialize(d3dDevice, SamplerStateDescConstants::Anisotropic);

	// Setup light technique:
	m_lightTechnique.SetVertexShader(&m_vertexShader);
	m_lightTechnique.SetPixelShader(&m_pixelShader);
	m_lightTechnique.VSSetConstantBuffer(m_perObjectConstantBuffer, 0);
	m_lightTechnique.PSSetConstantBuffer(m_perObjectConstantBuffer, 0);
	m_lightTechnique.PSSetConstantBuffer(m_perFrameConstantBuffer, 1);
	m_lightTechnique.PSSetSamplerState(m_samplerState, 0);
}

void LightEffect::Reset()
{
	m_samplerState.Reset();
	m_perFrameConstantBuffer.Reset();
	m_perObjectConstantBuffer.Reset();
	m_pixelShader.Reset();
	m_vertexShader.Reset();
}

void LightEffect::SetTexture(const Texture& texture)
{
	m_lightTechnique.PSSetTexture(texture, 0);
}

void LightEffect::UpdatePerObjectConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const PerObjectConstantBuffer& buffer) const
{
	m_perObjectConstantBuffer.Update(d3dDeviceContext, &buffer);
}
void LightEffect::UpdatePerFrameConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const PerFrameConstantBuffer& buffer) const
{
	m_perFrameConstantBuffer.Update(d3dDeviceContext, &buffer);
}

void LightEffect::Set(ID3D11DeviceContext1* d3dDeviceContext) const
{
	// Set technique:
	m_lightTechnique.Set(d3dDeviceContext);
}

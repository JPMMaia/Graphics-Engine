#include "stdafx.h"
#include "LightEffect.h"

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
	vector<D3D11_INPUT_ELEMENT_DESC> inputDesc = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// Initialize the vertex shader:
	m_vertexShader.Initialize(d3dDevice, L"LightVertexShader.cso", inputDesc);

	// Initialize the pixel shader:
	m_pixelShader.Initialize(d3dDevice, L"LightPixelShader.cso");

	// Initialize constant buffers:
	m_perObjectConstantBuffer.Initialize(d3dDevice, sizeof(PerObjectConstantBuffer));
	m_perFrameConstantBuffer.Initialize(d3dDevice, sizeof(PerFrameConstantBuffer));
}

void LightEffect::Reset()
{
	m_perFrameConstantBuffer.Reset();
	m_perObjectConstantBuffer.Reset();
	m_pixelShader.Reset();
	m_vertexShader.Reset();
}

void LightEffect::UpdatePerObjectConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const PerObjectConstantBuffer& buffer)
{
	m_perObjectConstantBuffer.Update(d3dDeviceContext, buffer);
}

void LightEffect::UpdatePerFrameConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const PerFrameConstantBuffer& buffer)
{
	m_perFrameConstantBuffer.Update(d3dDeviceContext, buffer);
}

void LightEffect::Set(ID3D11DeviceContext1* d3dDeviceContext)
{
	// Set vertex shader:
	m_vertexShader.Set(d3dDeviceContext);

	// Set constant buffers:
	m_perObjectConstantBuffer.VSSet(d3dDeviceContext, 0);
	m_perObjectConstantBuffer.PSSet(d3dDeviceContext, 0);
	m_perFrameConstantBuffer.PSSet(d3dDeviceContext, 1);

	// Set pixel shader:
	m_pixelShader.Set(d3dDeviceContext);
}
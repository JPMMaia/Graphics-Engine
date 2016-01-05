#include "stdafx.h"
#include "Technique.h"

using namespace GraphicsEngine;

Technique::Technique() :
	m_vertexShader(nullptr),
	m_pixelShader(nullptr),
	m_rasterizerState(nullptr)
{
}

void Technique::Set(ID3D11DeviceContext1* d3dDeviceContext) const
{
	// Set vertex shader:
	if (m_vertexShader)
		m_vertexShader->Set(d3dDeviceContext);
	else
		d3dDeviceContext->VSSetShader(nullptr, nullptr, 0);

	// Set pixel shader:
	if (m_pixelShader)
		m_pixelShader->Set(d3dDeviceContext);
	else
		d3dDeviceContext->PSSetShader(nullptr, nullptr, 0);

	// Set rasterizer state:
	if (m_rasterizerState)
		m_rasterizerState->Set(d3dDeviceContext);
	else
		d3dDeviceContext->RSSetState(nullptr);

	m_samplerStateArrays.Set(d3dDeviceContext);
	m_constantBufferArrays.Set(d3dDeviceContext);
}

void Technique::SetVertexShader(const VertexShader* vertexShader)
{
	m_vertexShader = vertexShader;
}
void Technique::SetPixelShader(const PixelShader* pixelShader)
{
	m_pixelShader = pixelShader;
}

void Technique::SetRasterizerState(const RasterizerState* rasterizerState)
{
	m_rasterizerState = rasterizerState;
}

void Technique::VSSetConstantBuffer(ID3D11Buffer* constantBuffer, uint32_t slot)
{
	m_constantBufferArrays.VSSetElement(std::forward<ID3D11Buffer*>(constantBuffer), slot);
}
void Technique::PSSetConstantBuffer(ID3D11Buffer* constantBuffer, uint32_t slot)
{
	m_constantBufferArrays.PSSetElement(std::forward<ID3D11Buffer*>(constantBuffer), slot);
}

void Technique::VSSetSamplerState(const SamplerState& samplerState, uint32_t slot)
{
	m_samplerStateArrays.VSSetElement(samplerState.Get(), slot);
}
void Technique::PSSetSamplerState(const SamplerState& samplerState, uint32_t slot)
{
	m_samplerStateArrays.PSSetElement(samplerState.Get(), slot);
}
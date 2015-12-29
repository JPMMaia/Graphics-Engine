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

	SetConstantBuffers(d3dDeviceContext);
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

void Technique::SetVSConstantBuffer(ConstantBuffer* constantBuffer, uint32_t slot)
{
	m_vertexShaderConstantBuffers[slot] = constantBuffer;
}

void Technique::SetPSConstantBuffer(ConstantBuffer* constantBuffer, uint32_t slot)
{
	m_pixelShaderConstantBuffers[slot] = constantBuffer;
}

void Technique::SetConstantBuffers(ID3D11DeviceContext1* d3dDeviceContext) const
{
	// Set vertex shader constant buffers:
	for(auto element : m_vertexShaderConstantBuffers)
		element.second->VSSet(d3dDeviceContext, element.first);

	// Set pixel shader constant buffers:
	for (auto element : m_pixelShaderConstantBuffers)
		element.second->PSSet(d3dDeviceContext, element.first);
}
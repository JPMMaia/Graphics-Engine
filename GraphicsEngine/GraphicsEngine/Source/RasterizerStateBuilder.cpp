#include "stdafx.h"
#include "RasterizerStateBuilder.h"

using namespace GraphicsEngine;

RasterizerStateBuilder::RasterizerStateBuilder()
{
	// Setting defaults:
	m_rasterizerDesc.AntialiasedLineEnable = false;
	m_rasterizerDesc.CullMode = D3D11_CULL_BACK;
	m_rasterizerDesc.DepthBias = 0;
	m_rasterizerDesc.DepthBiasClamp = 0.0f;
	m_rasterizerDesc.DepthClipEnable = true;
	m_rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	m_rasterizerDesc.FrontCounterClockwise = false;
	m_rasterizerDesc.MultisampleEnable = false;
	m_rasterizerDesc.ScissorEnable = false;
	m_rasterizerDesc.SlopeScaledDepthBias = 0.0f;
}

void RasterizerStateBuilder::SetCullMode(D3D11_CULL_MODE cullMode)
{
	m_rasterizerDesc.CullMode = cullMode;
}
void RasterizerStateBuilder::SetFillMode(D3D11_FILL_MODE fillMode)
{
	m_rasterizerDesc.FillMode = fillMode;
}
void RasterizerStateBuilder::SetFrontCounterClockwise(bool frontCounterClockwise)
{
	m_rasterizerDesc.FrontCounterClockwise = frontCounterClockwise;
}

void RasterizerStateBuilder::Create(ID3D11Device* d3dDevice, RasterizerState& rasterizerState) const
{
	Create(d3dDevice, rasterizerState.GetAddressOf());
}

void RasterizerStateBuilder::Create(ID3D11Device* d3dDevice, ID3D11RasterizerState** rasterizerState) const
{
	DX::ThrowIfFailed(
		d3dDevice->CreateRasterizerState(&m_rasterizerDesc, rasterizerState)
		);
}

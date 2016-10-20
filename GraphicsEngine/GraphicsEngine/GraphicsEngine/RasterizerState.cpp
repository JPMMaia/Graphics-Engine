#include "stdafx.h"
#include "RasterizerState.h"

using namespace Common;
using namespace GraphicsEngine;

RasterizerState::RasterizerState()
{
}

RasterizerState::RasterizerState(ID3D11Device* d3dDevice, const D3D11_RASTERIZER_DESC& rasterizerDesc)
{
	Initialize(d3dDevice, rasterizerDesc);
}

void RasterizerState::Initialize(ID3D11Device* d3dDevice, const D3D11_RASTERIZER_DESC& rasterizerDesc)
{
	ThrowIfFailed(
		d3dDevice->CreateRasterizerState(&rasterizerDesc, m_rasterizerState.GetAddressOf())
		);
}

void RasterizerState::Reset()
{
	m_rasterizerState.Reset();
}

void RasterizerState::Set(ID3D11DeviceContext* d3dDeviceContext) const
{
	// Set the rasterizer state.
	d3dDeviceContext->RSSetState(m_rasterizerState.Get());
}

ID3D11RasterizerState* RasterizerState::Get() const
{
	return m_rasterizerState.Get();
}

ID3D11RasterizerState* const* RasterizerState::GetAddressOf() const
{
	return m_rasterizerState.GetAddressOf();
}

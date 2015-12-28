#include "stdafx.h"
#include "RasterizerState.h"

using namespace GraphicsEngine;

RasterizerState::RasterizerState()
{
}

void RasterizerState::Reset()
{
	m_rasterizerState.Reset();
}

void RasterizerState::Set(ID3D11DeviceContext1* d3dDeviceContext) const
{
	// Set the rasterizer state.
	d3dDeviceContext->RSSetState(m_rasterizerState.Get());
}

ID3D11RasterizerState* RasterizerState::Get() const
{
	return m_rasterizerState.Get();
}

ID3D11RasterizerState** RasterizerState::GetAddressOf()
{
	return m_rasterizerState.GetAddressOf();
}

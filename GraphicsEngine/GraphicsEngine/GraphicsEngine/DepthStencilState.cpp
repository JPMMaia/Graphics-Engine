#include "stdafx.h"
#include "DepthStencilState.h"

using namespace Common;
using namespace GraphicsEngine;

DepthStencilState::DepthStencilState(ID3D11Device* d3dDevice, const D3D11_DEPTH_STENCIL_DESC& depthStencilDesc)
{
	Initialize(d3dDevice, depthStencilDesc);
}

void DepthStencilState::Initialize(ID3D11Device* d3dDevice, const D3D11_DEPTH_STENCIL_DESC& depthStencilDesc)
{
	ThrowIfFailed(
		d3dDevice->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.GetAddressOf())
	);
}

void DepthStencilState::Reset()
{
	m_depthStencilState.Reset();
}

void DepthStencilState::Set(ID3D11DeviceContext* d3dDeviceContext) const
{
	d3dDeviceContext->OMSetDepthStencilState(m_depthStencilState.Get(), 1);
}

ID3D11DepthStencilState* DepthStencilState::Get() const
{
	return m_depthStencilState.Get();
}

ID3D11DepthStencilState* const* DepthStencilState::GetAddressOf() const
{
	return m_depthStencilState.GetAddressOf();
}

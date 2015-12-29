#include "stdafx.h"
#include "SamplerState.h"

using namespace GraphicsEngine;

SamplerState::SamplerState()
{
}

SamplerState::SamplerState(ID3D11Device* d3dDevice, const D3D11_SAMPLER_DESC& samplerDesc)
{
	Initialize(d3dDevice, samplerDesc);
}

void SamplerState::Initialize(ID3D11Device* d3dDevice, const D3D11_SAMPLER_DESC& samplerDesc)
{
	// Create sampler state:
	DX::ThrowIfFailed(
		d3dDevice->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf())
		);
}

void SamplerState::Reset()
{
	m_samplerState.Reset();
}

void SamplerState::VSSet(ID3D11DeviceContext1* d3dDeviceContext, uint32_t slot) const
{
	d3dDeviceContext->VSSetSamplers(slot, 1, m_samplerState.GetAddressOf());
}
void SamplerState::PSSet(ID3D11DeviceContext1* d3dDeviceContext, uint32_t slot) const
{
	d3dDeviceContext->PSSetSamplers(slot, 1, m_samplerState.GetAddressOf());
}

ID3D11SamplerState* SamplerState::Get() const
{
	return m_samplerState.Get();
}
ID3D11SamplerState* const* SamplerState::GetAddressOf() const
{
	return m_samplerState.GetAddressOf();
}
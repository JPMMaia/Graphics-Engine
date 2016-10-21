#include "stdafx.h"
#include "BlendState.h"

using namespace GraphicsEngine;

BlendState::BlendState(ID3D11Device1* device, const D3D11_BLEND_DESC1& blendDesc, const std::array<float, 4>& blendFactor, UINT sampleMask) :
	m_blendFactor(blendFactor),
	m_sampleMask(sampleMask)
{
	Common::ThrowIfFailed(device->CreateBlendState1(&blendDesc, m_blendState.GetAddressOf()));
}

void BlendState::Set(ID3D11DeviceContext* deviceContext) const
{
	deviceContext->OMSetBlendState(m_blendState.Get(), m_blendFactor.data(), m_sampleMask);
}

ID3D11BlendState1* BlendState::Get() const
{
	return m_blendState.Get();
}
ID3D11BlendState1* const* BlendState::GetAddressOf() const
{
	return m_blendState.GetAddressOf();
}

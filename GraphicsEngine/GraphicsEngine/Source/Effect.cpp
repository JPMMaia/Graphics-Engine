#include "stdafx.h"
#include "Effect.h"

using namespace GraphicsEngine;
using namespace GraphicsEngine::EffectTypes;

VSEffect::VSEffect() noexcept
{
}
const VertexShader& VSEffect::GetShader() const noexcept
{
	return m_vertexShader;
}

HSEffect::HSEffect() noexcept
{
}
HSEffect::HSEffect(ID3D11Device* d3dDevice, const std::wstring& filename) :
	m_hullShader(d3dDevice, filename)
{
}
const HullShader& HSEffect::GetShader() const noexcept
{
	return m_hullShader;
}

DSEffect::DSEffect() noexcept
{
}
DSEffect::DSEffect(ID3D11Device* d3dDevice, const std::wstring& filename) :
	m_domainShader(d3dDevice, filename)
{
}
const DomainShader& DSEffect::GetShader() const noexcept
{
	return m_domainShader;
}

PSEffect::PSEffect() noexcept
{
}
PSEffect::PSEffect(ID3D11Device* d3dDevice, const std::wstring& filename) :
	m_pixelShader(d3dDevice, filename)
{
}
const PixelShader& PSEffect::GetShader() const noexcept
{
	return m_pixelShader;
}
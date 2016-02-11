#include "stdafx.h"
#include "Effect.h"

using namespace GraphicsEngine;
using namespace GraphicsEngine::EffectTypes;

HSEffect::HSEffect(ID3D11Device* d3dDevice, const std::wstring& filename) :
	m_hullShader(d3dDevice, filename)
{
}

DSEffect::DSEffect(ID3D11Device* d3dDevice, const std::wstring& filename) :
	m_domainShader(d3dDevice, filename)
{
}

PSEffect::PSEffect(ID3D11Device* d3dDevice, const std::wstring& filename) :
	m_pixelShader(d3dDevice, filename)
{
}

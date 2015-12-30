#include "stdafx.h"
#include "EffectManager.h"

using namespace GraphicsEngine;

EffectManager::EffectManager()
{
}
EffectManager::EffectManager(ID3D11Device* d3dDevice) :
	m_lightEffect(d3dDevice)
{
}

void EffectManager::Initialize(ID3D11Device* d3dDevice)
{
	m_lightEffect.Initialize(d3dDevice);
}

void EffectManager::Reset()
{
	m_lightEffect.Reset();
}

LightEffect& EffectManager::GetLightEffect()
{
	return m_lightEffect;
}
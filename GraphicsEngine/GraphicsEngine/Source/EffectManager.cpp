#include "stdafx.h"
#include "EffectManager.h"

using namespace GraphicsEngine;

EffectManager::EffectManager()
{
}
EffectManager::EffectManager(ID3D11Device* d3dDevice) :
	m_colorEffect(d3dDevice),
	m_lightEffect(d3dDevice),
	m_terrainEffect(d3dDevice)
{
}

void EffectManager::Reset()
{
	m_terrainEffect.Reset();
	m_lightEffect.Reset();
	m_colorEffect.Reset();
}

ColorEffect& EffectManager::GetColorEffect()
{
	return m_colorEffect;
}
LightEffect& EffectManager::GetLightEffect()
{
	return m_lightEffect;
}
TerrainEffect& EffectManager::GetTerrainEffect()
{
	return m_terrainEffect;
}
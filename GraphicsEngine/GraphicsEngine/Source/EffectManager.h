#pragma once

#include "LightEffect.h"
#include "TerrainEffect.h"

namespace GraphicsEngine
{
	class EffectManager
	{
	public:
		EffectManager();
		EffectManager(ID3D11Device* d3dDevice);

		void Initialize(ID3D11Device* d3dDevice);
		void Reset();

		LightEffect& GetLightEffect();
		TerrainEffect& GetTerrainEffect();

	private:
		LightEffect m_lightEffect;
		TerrainEffect m_terrainEffect;
	};
}

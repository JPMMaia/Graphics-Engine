#pragma once

#include "ColorEffect.h"
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

		ColorEffect& GetColorEffect();
		LightEffect& GetLightEffect();
		TerrainEffect& GetTerrainEffect();

	private:
		ColorEffect m_colorEffect;
		LightEffect m_lightEffect;
		TerrainEffect m_terrainEffect;
	};
}

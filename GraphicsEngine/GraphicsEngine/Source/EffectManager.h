#pragma once

#include "LightEffect.h"

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

	private:
		LightEffect m_lightEffect;
	};
}

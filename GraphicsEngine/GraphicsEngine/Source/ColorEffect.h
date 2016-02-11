#pragma once

#include "Effect.h"

namespace GraphicsEngine
{
	class ColorEffect : public Effect<EffectTypes::VSEffect, EffectTypes::PSEffect>
	{
	public:
		explicit ColorEffect(ID3D11Device* d3dDevice);

	private:
		static const std::array<D3D11_INPUT_ELEMENT_DESC, 6> s_INPUT_ELEMENT_DESCRIPTION;
	};
}

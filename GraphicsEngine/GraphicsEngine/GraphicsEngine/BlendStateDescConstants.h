#pragma once

#include <d3d11_2.h>

namespace GraphicsEngine
{
	namespace BlendStateDescConstants
	{
		D3D11_BLEND_DESC1 Default();
		D3D11_BLEND_DESC1 Transparent();
		D3D11_BLEND_DESC1 AdditiveBlend();
	}
}

#pragma once

#include <d3d11_4.h>

namespace GraphicsEngine
{
	namespace RasterizerStateDescConstants
	{
		static constexpr D3D11_RASTERIZER_DESC Default =
		{
			D3D11_FILL_WIREFRAME,					// FillMode
			D3D11_CULL_BACK,						// CullMode
			false,									// FrontCounterClockwise
			D3D11_DEFAULT_DEPTH_BIAS,				// DepthBias
			D3D11_DEFAULT_DEPTH_BIAS_CLAMP,			// DepthBiasClamp
			D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,	// SlopeScaledDepthBias
			true,									// DepthClipEnable
			false,									// ScissorEnable
			false,									// MultisampleEnable
			false									// AntialiasedLineEnable
		};
	};
};
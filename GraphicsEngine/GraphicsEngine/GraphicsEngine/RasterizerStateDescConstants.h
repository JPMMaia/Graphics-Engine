#pragma once

#include <d3d11_2.h>

namespace GraphicsEngine
{
	namespace RasterizerStateDescConstants
	{
		static constexpr D3D11_RASTERIZER_DESC Default =
		{
			D3D11_FILL_MODE::D3D11_FILL_SOLID,		// FillMode
			D3D11_CULL_MODE::D3D11_CULL_BACK,		// CullMode
			false,									// FrontCounterClockwise
			D3D11_DEFAULT_DEPTH_BIAS,				// DepthBias
			D3D11_DEFAULT_DEPTH_BIAS_CLAMP,			// DepthBiasClamp
			D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,	// SlopeScaledDepthBias
			true,									// DepthClipEnable
			false,									// ScissorEnable
			false,									// MultisampleEnable
			false									// AntialiasedLineEnable
		};

		static constexpr D3D11_RASTERIZER_DESC Wireframe =
		{
			D3D11_FILL_MODE::D3D11_FILL_WIREFRAME,	// FillMode
			D3D11_CULL_MODE::D3D11_CULL_BACK,		// CullMode
			false,									// FrontCounterClockwise
			D3D11_DEFAULT_DEPTH_BIAS,				// DepthBias
			D3D11_DEFAULT_DEPTH_BIAS_CLAMP,			// DepthBiasClamp
			D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,	// SlopeScaledDepthBias
			true,									// DepthClipEnable
			false,									// ScissorEnable
			false,									// MultisampleEnable
			false									// AntialiasedLineEnable
		};

		static constexpr D3D11_RASTERIZER_DESC NoCulling =
		{
			D3D11_FILL_MODE::D3D11_FILL_SOLID,		// FillMode
			D3D11_CULL_MODE::D3D11_CULL_NONE,		// CullMode
			false,									// FrontCounterClockwise
			D3D11_DEFAULT_DEPTH_BIAS,				// DepthBias
			D3D11_DEFAULT_DEPTH_BIAS_CLAMP,			// DepthBiasClamp
			D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,	// SlopeScaledDepthBias
			true,									// DepthClipEnable
			false,									// ScissorEnable
			false,									// MultisampleEnable
			false									// AntialiasedLineEnable
		};

		static constexpr D3D11_RASTERIZER_DESC CullFront =
		{
			D3D11_FILL_MODE::D3D11_FILL_SOLID,		// FillMode
			D3D11_CULL_MODE::D3D11_CULL_FRONT,		// CullMode
			false,									// FrontCounterClockwise
			D3D11_DEFAULT_DEPTH_BIAS,				// DepthBias
			D3D11_DEFAULT_DEPTH_BIAS_CLAMP,			// DepthBiasClamp
			D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,	// SlopeScaledDepthBias
			true,									// DepthClipEnable
			false,									// ScissorEnable
			false,									// MultisampleEnable
			false									// AntialiasedLineEnable
		};

		static constexpr D3D11_RASTERIZER_DESC Shadows =
		{
			D3D11_FILL_MODE::D3D11_FILL_SOLID,		// FillMode
			D3D11_CULL_MODE::D3D11_CULL_BACK,		// CullMode
			false,									// FrontCounterClockwise
			20000,									// DepthBias
			0.0f,									// DepthBiasClamp
			1.0f,									// SlopeScaledDepthBias
			true,									// DepthClipEnable
			false,									// ScissorEnable
			false,									// MultisampleEnable
			false									// AntialiasedLineEnable
		};
	};
};
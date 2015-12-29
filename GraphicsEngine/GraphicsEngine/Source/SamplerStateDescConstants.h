#pragma once

#include <cfloat>
#include <d3d11_4.h>

namespace GraphicsEngine
{
	namespace SamplerStateDescConstants
	{
		static constexpr D3D11_SAMPLER_DESC Anisotropic =
		{
			D3D11_FILTER_ANISOTROPIC,		// Filter
			D3D11_TEXTURE_ADDRESS_CLAMP,	// AddressU
			D3D11_TEXTURE_ADDRESS_CLAMP,	// AddressV
			D3D11_TEXTURE_ADDRESS_CLAMP,	// AddressW
			0.0f,							// MipLODBias
			4,								// MaxAnisotropy
			D3D11_COMPARISON_NEVER,			// ComparisonFunc
			{ 1.0f, 1.0f, 1.0f, 1.0f },		// BorderColor
			-FLT_MAX,						// MinLOD
			FLT_MAX							// MaxLOD
		};
	};
};
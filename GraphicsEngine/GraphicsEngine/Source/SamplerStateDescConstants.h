#pragma once

#include <d3d11_2.h>

namespace GraphicsEngine
{
	namespace SamplerStateDescConstants
	{
		static constexpr D3D11_SAMPLER_DESC Anisotropic =
		{
			D3D11_FILTER_ANISOTROPIC,		// Filter
			D3D11_TEXTURE_ADDRESS_WRAP,		// AddressU
			D3D11_TEXTURE_ADDRESS_WRAP,		// AddressV
			D3D11_TEXTURE_ADDRESS_WRAP,		// AddressW
			0.0f,							// MipLODBias
			4,								// MaxAnisotropy
			D3D11_COMPARISON_ALWAYS,		// ComparisonFunc
			{ 1.0f, 1.0f, 1.0f, 1.0f },		// BorderColor
			0.0f,							// MinLOD
			FLT_MAX							// MaxLOD
		};
	};
};
#pragma once

#include <d3d11_2.h>
#include <cfloat>

namespace GraphicsEngine
{
	namespace SamplerStateDescConstants
	{
		static constexpr D3D11_SAMPLER_DESC PointWrap =
		{
			D3D11_FILTER_MIN_MAG_MIP_POINT, // Filter
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
		static constexpr D3D11_SAMPLER_DESC PointClamp =
		{
			D3D11_FILTER_MIN_MAG_MIP_POINT,// Filter
			D3D11_TEXTURE_ADDRESS_CLAMP,	// AddressU
			D3D11_TEXTURE_ADDRESS_CLAMP,	// AddressV
			D3D11_TEXTURE_ADDRESS_CLAMP,	// AddressW
			0.0f,							// MipLODBias
			4,								// MaxAnisotropy
			D3D11_COMPARISON_ALWAYS,		// ComparisonFunc
			{ 1.0f, 1.0f, 1.0f, 1.0f },		// BorderColor
			0.0f,							// MinLOD
			FLT_MAX							// MaxLOD
		};
		static constexpr D3D11_SAMPLER_DESC LinearWrap =
		{
			D3D11_FILTER_MIN_MAG_MIP_LINEAR,// Filter
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
		static constexpr D3D11_SAMPLER_DESC LinearClamp =
		{
			D3D11_FILTER_MIN_MAG_MIP_LINEAR,// Filter
			D3D11_TEXTURE_ADDRESS_CLAMP,	// AddressU
			D3D11_TEXTURE_ADDRESS_CLAMP,	// AddressV
			D3D11_TEXTURE_ADDRESS_CLAMP,	// AddressW
			0.0f,							// MipLODBias
			4,								// MaxAnisotropy
			D3D11_COMPARISON_ALWAYS,		// ComparisonFunc
			{ 1.0f, 1.0f, 1.0f, 1.0f },		// BorderColor
			0.0f,							// MinLOD
			FLT_MAX							// MaxLOD
		};
		static constexpr D3D11_SAMPLER_DESC AnisotropicWrap =
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
		static constexpr D3D11_SAMPLER_DESC AnisotropicClamp =
		{
			D3D11_FILTER_ANISOTROPIC,		// Filter
			D3D11_TEXTURE_ADDRESS_CLAMP,	// AddressU
			D3D11_TEXTURE_ADDRESS_CLAMP,	// AddressV
			D3D11_TEXTURE_ADDRESS_CLAMP,	// AddressW
			0.0f,							// MipLODBias
			4,								// MaxAnisotropy
			D3D11_COMPARISON_ALWAYS,		// ComparisonFunc
			{ 1.0f, 1.0f, 1.0f, 1.0f },		// BorderColor
			0.0f,							// MinLOD
			FLT_MAX							// MaxLOD
		};
		static constexpr D3D11_SAMPLER_DESC Shadows =
		{
			D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,	// Filter
			D3D11_TEXTURE_ADDRESS_BORDER,						// AddressU
			D3D11_TEXTURE_ADDRESS_BORDER,						// AddressV
			D3D11_TEXTURE_ADDRESS_BORDER,						// AddressW
			0.0f,												// MipLODBias
			16,													// MaxAnisotropy
			D3D11_COMPARISON_LESS_EQUAL,						// ComparisonFunc
			{ 1.0f, 1.0f, 1.0f, 1.0f },							// BorderColor
			0.0f,												// MinLOD
			FLT_MAX												// MaxLOD
		};
	};
};

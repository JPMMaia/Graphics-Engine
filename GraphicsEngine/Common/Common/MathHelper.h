#pragma once

#include <DirectXMath.h>
#include <DirectXPackedVector.h>

namespace GraphicsEngine
{
	namespace MathHelper
	{
		static DirectX::XMFLOAT4X4 Identity4x4()
		{
			static DirectX::XMFLOAT4X4 I(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

			return I;
		}

		static DirectX::PackedVector::XMHALF4 ConvertFloat4ToHalf4(const DirectX::XMFLOAT4& value)
		{
			using namespace DirectX;
			PackedVector::XMHALF4 half4;
			half4.x = PackedVector::XMConvertFloatToHalf(value.x);
			half4.y = PackedVector::XMConvertFloatToHalf(value.y);
			half4.z = PackedVector::XMConvertFloatToHalf(value.z);
			half4.w = PackedVector::XMConvertFloatToHalf(value.w);
			return half4;
		}

		template<typename Type>
		static Type LinearInterpolate(const Type& value1, const Type& value2, float blendFactor)
		{
			return (1.0f - blendFactor) * value1 + blendFactor * value2;
		}

		const auto Infinity = FLT_MAX;
	}
}

#pragma once

#include <DirectXMath.h>
#include <DirectXPackedVector.h>

namespace GraphicsEngine
{
	namespace MathHelper
	{
		DirectX::XMFLOAT4X4 Identity4x4();
		DirectX::PackedVector::XMHALF4 ConvertFloat4ToHalf4(const DirectX::XMFLOAT4& value);

		template<typename Type>
		Type LinearInterpolate(const Type& value1, const Type& value2, float blendFactor)
		{
			return (1.0f - blendFactor) * value1 + blendFactor * value2;
		}

		const auto Infinity = FLT_MAX;
	}
}

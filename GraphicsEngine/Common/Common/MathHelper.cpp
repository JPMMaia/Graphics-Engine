#include "MathHelper.h"

using namespace DirectX;
using namespace GraphicsEngine;

DirectX::XMFLOAT4X4 MathHelper::Identity4x4()
{
	static XMFLOAT4X4 I(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	return I;
}

DirectX::PackedVector::XMHALF4 MathHelper::ConvertFloat4ToHalf4(const DirectX::XMFLOAT4& value)
{
	using namespace DirectX;
	PackedVector::XMHALF4 half4;
	half4.x = PackedVector::XMConvertFloatToHalf(value.x);
	half4.y = PackedVector::XMConvertFloatToHalf(value.y);
	half4.z = PackedVector::XMConvertFloatToHalf(value.z);
	half4.w = PackedVector::XMConvertFloatToHalf(value.w);
	return half4;
}

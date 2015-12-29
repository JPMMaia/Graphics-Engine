#pragma once

#include <DirectXMath.h>

struct SpotLight
{
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT3 Specular;
	float Pad;
	DirectX::XMFLOAT3 Position;
	float Range;
	DirectX::XMFLOAT3 Direction;
	float Spot;
	DirectX::XMFLOAT3 Attenuation;
	float Pad2;
};

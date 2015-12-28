#pragma once

#include <DirectXMath.h>

struct PointLight
{
	DirectX::XMFLOAT4 Ambient; 
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT3 Specular;
	float Shininess;
	DirectX::XMFLOAT3 Position;
	float Range; 
	DirectX::XMFLOAT3 Attenuation;
	float Pad;
};
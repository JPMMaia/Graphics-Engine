#pragma once

#include <DirectXMath.h>

struct DirectionalLight
{
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT3 Specular;
	float Shininess;
	DirectX::XMFLOAT3 Direction; 
	float Pad;
}; 
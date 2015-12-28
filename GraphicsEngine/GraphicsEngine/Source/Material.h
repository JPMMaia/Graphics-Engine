#pragma once

#include "DirectXMath.h"

struct Material
{
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT3 Specular;
	float Shininess;
};
#pragma once

#include "DirectXMath.h"

struct Material
{
	/*Material() :
		Shininess(0.0f)
	{
	}

	Material(const DirectX::XMFLOAT4& ambient, const DirectX::XMFLOAT4& diffuse, const DirectX::XMFLOAT3& specular, float shininess) :
		Ambient(ambient),
		Diffuse(diffuse),
		Specular(specular),
		Shininess(shininess)
	{
	}*/

	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;
	float Shininess;
	DirectX::XMFLOAT3 Pad;
};
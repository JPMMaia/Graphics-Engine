#pragma once

#include <DirectXMath.h>

namespace GraphicsEngine
{
	struct DirectionalLight
	{
		DirectX::XMFLOAT4 Ambient;
		DirectX::XMFLOAT4 Diffuse;
		DirectX::XMFLOAT4 Specular;
		DirectX::XMFLOAT3 Direction;
		float Pad;
	};
}
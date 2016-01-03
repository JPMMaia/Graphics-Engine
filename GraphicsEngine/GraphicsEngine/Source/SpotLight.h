#pragma once

#include <DirectXMath.h>

namespace GraphicsEngine
{
	struct SpotLight
	{
		DirectX::XMFLOAT4 Ambient;
		DirectX::XMFLOAT4 Diffuse;
		DirectX::XMFLOAT4 Specular;
		DirectX::XMFLOAT3 Position;
		float Range;
		DirectX::XMFLOAT3 Direction;
		float Spot;
		DirectX::XMFLOAT3 Attenuation;
		float Pad;
	};
}
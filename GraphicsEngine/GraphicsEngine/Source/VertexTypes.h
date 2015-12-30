#pragma once

#include <DirectXMath.h>

namespace GraphicsEngine
{
	struct VertexPositionNormal
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Normal;
	};

	struct VertexPositionNormalTexture
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT2 TextureCoordinate;
	};
}

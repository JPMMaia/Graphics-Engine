#pragma once

#include <DirectXMath.h>

namespace GraphicsEngine
{
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT4 Color;
	};

	struct VertexPositionTexture
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 TextureCoordinate;
	};

	struct VertexPositionTextureNormalTangent
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT3 Tangent;
		DirectX::XMFLOAT2 TextureCoordinate;
	};
}

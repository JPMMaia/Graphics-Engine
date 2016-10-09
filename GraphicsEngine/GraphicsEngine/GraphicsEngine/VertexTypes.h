#pragma once

#include <DirectXMath.h>

namespace GraphicsEngine
{
	namespace VertexTypes
	{
		struct ColorVertexType
		{
			DirectX::XMFLOAT3 Position;
			DirectX::XMFLOAT4 Color;
		};

		struct NormalVertexType
		{
			DirectX::XMFLOAT3 Position;
			DirectX::XMFLOAT3 Normal;
		};

		struct NormalTextureVertexType
		{
			DirectX::XMFLOAT3 Position;
			DirectX::XMFLOAT3 Normal;
			DirectX::XMFLOAT2 TextureCoordinates;

			NormalTextureVertexType()
			{
			}
			NormalTextureVertexType(float positionX, float positionY, float positionZ, float normalX, float normalY, float normalZ, float textureCoordinatesU, float textureCoordinatesV) :
				Position(positionX, positionY, positionZ),
				Normal(normalX, normalY, normalZ),
				TextureCoordinates(textureCoordinatesU, textureCoordinatesV)
			{
			}
		};

		using DefaultVertexType = NormalTextureVertexType;
	}
}

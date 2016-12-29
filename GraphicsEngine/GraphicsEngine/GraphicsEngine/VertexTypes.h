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

		struct BillboardVertexType
		{
			DirectX::XMFLOAT3 Center;
			DirectX::XMFLOAT2 Extents;
		};

		struct NormalVertexType
		{
			DirectX::XMFLOAT3 Position;
			DirectX::XMFLOAT3 Normal;
		};

		struct TextureVertexType
		{
			DirectX::XMFLOAT3 Position;
			DirectX::XMFLOAT2 TextureCoordinates;
		};

		struct NormalTextureVertexType
		{
			DirectX::XMFLOAT3 Position;
			DirectX::XMFLOAT3 Normal;
			DirectX::XMFLOAT2 TextureCoordinates;

			NormalTextureVertexType() = default;
			NormalTextureVertexType(float positionX, float positionY, float positionZ, float normalX, float normalY, float normalZ, float textureCoordinatesU, float textureCoordinatesV) :
				Position(positionX, positionY, positionZ),
				Normal(normalX, normalY, normalZ),
				TextureCoordinates(textureCoordinatesU, textureCoordinatesV)
			{
			}
		};

		struct NormalTextureTangentVertexType
		{
			DirectX::XMFLOAT3 Position;
			DirectX::XMFLOAT3 Normal;
			DirectX::XMFLOAT2 TextureCoordinates;
			DirectX::XMFLOAT3 Tangent;

			NormalTextureTangentVertexType() = default;
			NormalTextureTangentVertexType(float positionX, float positionY, float positionZ, float normalX, float normalY, float normalZ, float textureCoordinatesU, float textureCoordinatesV, float tangentUx, float tangentUy, float tangentUz) :
				Position(positionX, positionY, positionZ),
				Normal(normalX, normalY, normalZ),
				TextureCoordinates(textureCoordinatesU, textureCoordinatesV),
				Tangent(tangentUx, tangentUy, tangentUz)
			{
			}
		};

		using DefaultVertexType = NormalTextureTangentVertexType;
	}
}

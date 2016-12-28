#include "stdafx.h"
#include "GeometryGenerator2.h"

using namespace DirectX;
using namespace GraphicsEngine;

GeometryGenerator2::Vertex::Vertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT3& tangentU, const DirectX::XMFLOAT2& textureCoordinates) :
	Position(position),
	Normal(normal),
	TangentU(tangentU),
	TextureCoordinates(textureCoordinates)
{
}

GeometryGenerator2::MeshData GeometryGenerator2::CreateRectangle(float originX, float originY, float width, float height, float depth)
{
	MeshData meshData;

	meshData.Vertices = 
	{
		{ XMFLOAT3(originX, originY - height, depth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(originX, originY, depth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(originX + width, originY, depth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(originX + width, originY - height, depth), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }
	};

	meshData.Indices = 
	{
		0, 1, 2,
		0, 2, 3,
	};

	return meshData;
}

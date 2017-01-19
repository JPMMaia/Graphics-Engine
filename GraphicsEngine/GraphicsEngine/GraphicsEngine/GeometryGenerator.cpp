#include "stdafx.h"
#include "GeometryGenerator.h"

using namespace DirectX;
using namespace GraphicsEngine;

GeometryGenerator::Vertex::Vertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT3& tangentU, const DirectX::XMFLOAT2& textureCoordinates) :
	Position(position),
	Normal(normal),
	Tangent(tangentU),
	TextureCoordinates(textureCoordinates)
{
}

GeometryGenerator::MeshData GeometryGenerator::CreateRectangle(float originX, float originY, float width, float height, float depth)
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

void GeometryGenerator::CalculateNormalAndTangentVectors(MeshData& meshData, uint32_t verticesPerFace)
{
	const auto& vertices = meshData.Vertices;
	const auto& indices = meshData.Indices;

	for(uint32_t i = 0; i < meshData.Indices.size(); i += verticesPerFace)
	{
		auto vertex0 = vertices[indices[i]];
		auto vertex1 = vertices[indices[i + 1]];
		auto vertex2 = vertices[indices[i + 2]];

		XMFLOAT3 tangent, bitangent, normal;
		CalculateTangentBitangentNormal(vertex0.Position, vertex1.Position, vertex2.Position, vertex0.TextureCoordinates, vertex1.TextureCoordinates, vertex2.TextureCoordinates, tangent, bitangent, normal);

		// TODO
	}
}

void GeometryGenerator::CalculateTangentBitangentNormal(const XMFLOAT3& position0, const XMFLOAT3& position1, const XMFLOAT3& position2, const XMFLOAT2& textureCoordinates0, const XMFLOAT2 textureCoordidanates1, const XMFLOAT2 textureCoordinates2, XMFLOAT3& tangent, XMFLOAT3& bitangent, XMFLOAT3& normal)
{
	// [ deltaU0 deltaV0 ] [ Tx Ty Tz ]     [ edge0.x edge0.y edge0.z ]
	// [                 ] [          ]  =  [                         ]
	// [ deltaU1 deltaV1 ] [ Bx By Bz ]     [ edge1.x edge1.y edge1.z ]

	// Set texture edge matrix:
	auto deltaUVMatrix = XMMatrixSet(
		textureCoordidanates1.x - textureCoordinates0.x, textureCoordidanates1.y - textureCoordinates0.y, 0.0f, 0.0f,
		textureCoordinates2.x - textureCoordinates0.x, textureCoordinates2.y - textureCoordinates0.y, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	// Calculate inverse of texture edge matrix:
	auto determinant = XMMatrixDeterminant(deltaUVMatrix);
	auto deltaUVInverseMatrix = XMMatrixInverse(&determinant, deltaUVMatrix);

	// Set edge matrix:
	auto edgeMatrix = XMMatrixSet(
		position1.x - position0.x, position1.y - position0.y, position1.z - position0.z, 0.0f,
		position2.x - position0.x, position2.y - position0.y, position2.z - position0.z, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	// Calculate the tangent and binormal matrix:
	XMFLOAT3X3 tangentBinormalMatrix;
	XMStoreFloat3x3(&tangentBinormalMatrix, XMMatrixMultiply(deltaUVInverseMatrix, edgeMatrix));

	auto tangentVector = XMVector3Normalize(XMVectorSet(tangentBinormalMatrix(0, 0), tangentBinormalMatrix(0, 1), tangentBinormalMatrix(0, 2), 0.0f));
	auto bitangentVector = XMVector3Normalize(XMVectorSet(tangentBinormalMatrix(1, 0), tangentBinormalMatrix(1, 1), tangentBinormalMatrix(1, 2), 0.0f));
	auto normalVector = XMVector3Cross(tangentVector, bitangentVector);

	XMStoreFloat3(&tangent, tangentVector);
	XMStoreFloat3(&bitangent, bitangentVector);
	XMStoreFloat3(&normal, normalVector);
}

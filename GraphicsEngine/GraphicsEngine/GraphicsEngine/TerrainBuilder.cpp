#include "stdafx.h"
#include "TerrainBuilder.h"
#include "VertexTypes.h"

using namespace DirectX;
using namespace GraphicsEngine;
using namespace std;

GeometryGenerator2::MeshData TerrainBuilder::CreateTerrain(const D3DBase& d3dBase, float width, float depth, uint32_t xCellCount, uint32_t zCellCount)
{
	GeometryGenerator2::MeshData output;

	auto xVertexCount = xCellCount + 1;
	auto zVertexCount = zCellCount + 1;
	auto vertexCount = xVertexCount * zVertexCount;
	auto halfWidth = width / 2.0f;
	auto halfDepth = depth / 2.0f;
	auto dx = width / xCellCount;
	auto dz = depth / zCellCount;
	auto du = 1.0f / xCellCount;
	auto dv = 1.0f / zCellCount;

	// Calculate vertices:
	output.Vertices.resize(vertexCount);
	for (uint32_t i = 0; i < zVertexCount; i++)
	{
		auto z = halfDepth - i * dz;
		auto v = i * dv;
		for (uint32_t j = 0; j < xVertexCount; j++)
		{
			auto x = -halfWidth + j * dx;
			auto u = j * du;
			auto index = i * xVertexCount + j;

			auto& vertex = output.Vertices[index];
			vertex.Position = XMFLOAT3(x, 0.0f, z);
			vertex.TextureCoordinates = XMFLOAT2(u, v);
		}
	}

	// Calculate the indices for quads:
	auto faceCount = xCellCount * zCellCount;
	uint32_t faceIndex = 0;
	output.Indices.resize(faceCount * 4);
	for (SIZE_T i = 0; i < zCellCount; i++)
	{
		auto row0Offset = i * xVertexCount;
		auto row1Offset = (i + 1) * zVertexCount;

		for (SIZE_T j = 0; j < xCellCount; j++)
		{
			auto index0 = row0Offset + j;
			auto index1 = row1Offset + j;
			auto index2 = index0 + 1;
			auto index3 = index1 + 1;

			output.Indices[faceIndex++] = static_cast<unsigned int>(index0);
			output.Indices[faceIndex++] = static_cast<unsigned int>(index1);
			output.Indices[faceIndex++] = static_cast<unsigned int>(index2);
			output.Indices[faceIndex++] = static_cast<unsigned int>(index3);
		}
	}

	return output;
}
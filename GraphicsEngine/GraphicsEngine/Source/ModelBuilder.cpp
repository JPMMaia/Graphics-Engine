#include "stdafx.h"
#include "ModelBuilder.h"
#include "X3DParser/X3DParser.h"
#include "LightModel.h"
#include "MathHelper.h"

#include <array>

using namespace DirectX;
using namespace GraphicsEngine;
using namespace std;

ModelBuilder::ModelBuilder(TextureManager& textureManager) :
	m_textureManager(textureManager)
{
}

LightModel ModelBuilder::CreateFromX3D(ID3D11Device* d3dDevice, const std::wstring& x3dFilename, const vector<LightEffect::InstanceData>& instancedData) const
{
	// Parse x3d filename:
	X3DParser parser(x3dFilename);

	auto& shape = parser.GetShape();

	// Create vertices:
	auto& indexedTriangleSet = shape.indexedTriangleSet;
	auto& coordinates = indexedTriangleSet.coordinate;
	auto& normals = indexedTriangleSet.normal;
	auto& textureCoordinates = indexedTriangleSet.textureCoordinate;

	auto coordinate = coordinates.begin();
	auto normal = normals.begin();
	auto textureCoordinate = textureCoordinates.begin();

	unsigned int vertexCount = coordinates.size() / 3;
	vector<VertexPositionTextureNormalTangent> vertices(vertexCount);
	for (unsigned int i = 0; i < vertexCount; i++)
	{
		auto& vertex = vertices[i];
		vertex.Position.x = *coordinate++;
		vertex.Position.y = *coordinate++;
		vertex.Position.z = *coordinate++;

		vertex.TextureCoordinate.x = *textureCoordinate++;
		vertex.TextureCoordinate.y = *textureCoordinate++;

		vertex.Normal.x = *normal++;
		vertex.Normal.y = *normal++;
		vertex.Normal.z = *normal++;
	}

	// Create indices:
	vector<uint32_t> indices(indexedTriangleSet.index.cbegin(), indexedTriangleSet.index.cend());

	// Calculate tangent vectors:
	for (auto index = indices.cbegin(); index != indices.cend(); )
	{
		// Get vertices of a triangle:
		auto& vertex0 = vertices[*index++];
		auto& vertex1 = vertices[*index++];
		auto& vertex2 = vertices[*index++];

		// Calculate tangent vector:
		XMFLOAT3 tangent, binormal;
		MathHelper::CalculateTangentBinormal(
			vertex0.Position, vertex1.Position, vertex2.Position,
			vertex0.TextureCoordinate, vertex1.TextureCoordinate, vertex2.TextureCoordinate,
			tangent, binormal
			);

		// Add tangent vector, as all these vertices share the same tangent vector:
		auto tangentVector = XMLoadFloat3(&tangent);
		XMStoreFloat3(&vertex0.Tangent, XMVectorAdd(XMLoadFloat3(&vertex0.Tangent), tangentVector));
		XMStoreFloat3(&vertex1.Tangent, XMVectorAdd(XMLoadFloat3(&vertex1.Tangent), tangentVector));
		XMStoreFloat3(&vertex2.Tangent, XMVectorAdd(XMLoadFloat3(&vertex2.Tangent), tangentVector));
	}

	// Normalize tangent vectors:
	for (auto& vertex : vertices)
		XMStoreFloat3(&vertex.Tangent, XMVector3Normalize(XMLoadFloat3(&vertex.Tangent)));

	// Create texture appearance:
	auto& appearance = shape.appearance;
	auto& material = appearance.material;
	auto& imageTexture = appearance.imageTexture;
	m_textureManager.Create(d3dDevice, imageTexture.def, imageTexture.url);
	m_textureManager.Create(d3dDevice, L"CubeNormalMap", L"Resources/old_bricks_normal_map.dds");
	m_textureManager.Create(d3dDevice, L"CubeHeightMap", L"Resources/old_bricks_height_map.dds");
	auto alpha = 1.0f - material.transparency;
	TextureAppearance textureAppearance =
	{
		Material(
			XMFLOAT4(material.ambientInttensity, material.ambientInttensity, material.ambientInttensity, alpha),
			XMFLOAT4(material.diffuseColor.x, material.diffuseColor.y, material.diffuseColor.z, alpha),
			XMFLOAT4(material.specularColor.x, material.specularColor.y, material.specularColor.z, alpha),
			material.shininess
			),
		m_textureManager[imageTexture.def],
		m_textureManager[L"CubeNormalMap"],
		m_textureManager[L"CubeHeightMap"]
	};

	Subset subset = { 0, indices.size() };

	return LightModel(d3dDevice, vertices, indices, { subset }, { textureAppearance }, instancedData, D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
}

LightModel ModelBuilder::CreateLightCube(ID3D11Device* d3dDevice, const vector<LightEffect::InstanceData>& instancedData) const
{
	static const std::vector<VertexPositionTextureNormalTangent> vertices =
	{
		{ { -0.5f, -0.5f, -0.5f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 1.0f } },
		{ { 0.5f, -0.5f, -0.5f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f } },
		{ { -0.5f, 0.5f, -0.5f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { 0.5f, 0.5f, -0.5f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },

		{ { -0.5f, -0.5f, 0.5f },{ -1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f },{ -1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },
		{ { -0.5f, 0.5f, 0.5f },{ -1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 0.0f } },
		{ { -0.5f, 0.5f, -0.5f },{ -1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f } },

		{ { 0.5f, -0.5f, 0.5f },{ 0.0f, 0.0f, -1.0f },{ -1.0f, 0.0f, 0.0f },{ 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, 0.5f },{ 0.0f, 0.0f, -1.0f },{ -1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f } },
		{ { 0.5f, 0.5f, 0.5f },{ 0.0f, 0.0f, -1.0f },{ -1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { -0.5f, 0.5f, 0.5f },{ 0.0f, 0.0f, -1.0f },{ -1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },

		{ { 0.5f, -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, -1.0f },{ 1.0f, 1.0f } },
		{ { 0.5f, -0.5f, 0.5f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, -1.0f },{ 0.0f, 1.0f } },
		{ { 0.5f, 0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, -1.0f },{ 1.0f, 0.0f } },
		{ { 0.5f, 0.5f, 0.5f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, -1.0f },{ 0.0f, 0.0f } },

		{ { -0.5f, -0.5f, 0.5f },{ 0.0f, -1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 1.0f } },
		{ { 0.5f, -0.5f, 0.5f },{ 0.0f, -1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f },{ 0.0f, -1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { 0.5f, -0.5f, -0.5f },{ 0.0f, -1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },

		{ { -0.5f, 0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 1.0f } },
		{ { 0.5f, 0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f } },
		{ { -0.5f, 0.5f, 0.5f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { 0.5f, 0.5f, 0.5f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
	};
	static const std::vector<uint32_t> indices =
	{
		0, 1, 2, 3,
		4, 5, 6, 7,
		8, 9, 10, 11,
		12, 13, 14, 15,
		16, 17, 18, 19,
		20, 21, 22, 23,
	};
	static const std::vector<Subset> subsets =
	{
		{ 0, indices.size() }
	};

	m_textureManager.Create(d3dDevice, L"CubeDiffuseMap", L"Resources/old_bricks_diffuse_map.dds");
	m_textureManager.Create(d3dDevice, L"CubeNormalMap", L"Resources/old_bricks_normal_map.dds");
	m_textureManager.Create(d3dDevice, L"CubeHeightMap", L"Resources/old_bricks_height_map.dds");
	TextureAppearance textureAppearance =
	{
		{
			{ 0.8f, 0.8f, 0.8f, 1.0f },
			{ 0.8f, 0.8f, 0.8f, 1.0f },
			{ 0.8f, 0.8f, 0.8f, 1.0f },
			20.0f
		},
		m_textureManager[L"CubeDiffuseMap"],
		m_textureManager[L"CubeNormalMap"],
		m_textureManager[L"CubeHeightMap"]
	};

	return LightModel(d3dDevice, vertices, indices, subsets, { textureAppearance }, instancedData, D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
}

ColorModel ModelBuilder::CreateBox(ID3D11Device* d3dDevice, const BoundingBox& boundingBox, const vector<ColorEffect::InstancedDataType>& instancedData) const
{
	// Create vertices:
	auto color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	array<XMFLOAT3, 8> corners;
	boundingBox.GetCorners(corners.data());
	auto vertices = vector<VertexPositionColor>(8);
	for (size_t i = 0; i < vertices.size(); ++i)
		vertices[i] = { corners[i], color };

	// Create indices:
	vector<uint32_t> indices = 
	{
		// Back
		0, 1, 2,
		0, 2, 3,

		// Front
		4, 7, 5,
		5, 7, 6,

		// Left
		4, 0, 3,
		4, 3, 7,

		// Right
		5, 6, 1,
		1, 6, 2,

		// Bottom
		0, 4, 1,
		1, 4, 5,

		// Top
		7, 3, 6,
		6, 3, 2
	};
	vector<Subset> subsets = { { 0, indices.size() } };
	
	return ColorModel(d3dDevice, vertices, indices, subsets, instancedData, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

TerrainModel ModelBuilder::CreateTerrain(ID3D11Device* d3dDevice, float width, float depth, uint32_t xCellCount, uint32_t zCellCount) const
{
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
	vector<VertexPositionTexture> vertices(vertexCount);
	for (uint32_t i = 0; i < zVertexCount; i++)
	{
		auto z = halfDepth - i * dz;
		auto v = i * dv;
		for (uint32_t j = 0; j < xVertexCount; j++)
		{
			auto x = -halfWidth + j * dx;
			auto u = j * du;
			auto index = i * xVertexCount + j;

			auto& vertex = vertices[index];
			vertex.Position = XMFLOAT3(x, 0.0f, z);
			vertex.TextureCoordinate = XMFLOAT2(u, v);
		}
	}

	// Calculate the indices for quads:
	auto faceCount = xCellCount * zCellCount;
	uint32_t faceIndex = 0;
	vector<uint32_t> indices(faceCount * 4);
	for (size_t i = 0; i < zCellCount; i++)
	{
		auto row0Offset = i * xVertexCount;
		auto row1Offset = (i + 1) * zVertexCount;

		for (size_t j = 0; j < xCellCount; j++)
		{
			auto index0 = row0Offset + j;
			auto index1 = row1Offset + j;
			auto index2 = index0 + 1;
			auto index3 = index1 + 1;

			indices[faceIndex++] = index0;
			indices[faceIndex++] = index1;
			indices[faceIndex++] = index2;
			indices[faceIndex++] = index3;
		}
	}
	static const vector<Subset> subsets =
	{
		{ 0, indices.size() }
	};

	m_textureManager.Create(d3dDevice, L"CubeDiffuseMap", L"Resources/old_bricks_diffuse_map.dds");
	m_textureManager.Create(d3dDevice, L"CubeNormalMap", L"Resources/old_bricks_normal_map.dds");
	m_textureManager.Create(d3dDevice, L"TerrainHeightMap", L"Resources/terrain_height_map.dds");
	TextureAppearance textureAppearance =
	{
		{
			{ 0.8f, 0.8f, 0.8f, 1.0f },
			{ 0.8f, 0.8f, 0.8f, 1.0f },
			{ 0.8f, 0.8f, 0.8f, 1.0f },
		20.0f
		},
		m_textureManager[L"CubeDiffuseMap"],
		m_textureManager[L"CubeNormalMap"],
		m_textureManager[L"TerrainHeightMap"]
	};
	static const vector<LightEffect::InstanceData> instancedData =
	{
		{
			XMFLOAT4X4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			)
		}
	};

	return TerrainModel(d3dDevice, vertices, indices, subsets, { textureAppearance }, D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
}

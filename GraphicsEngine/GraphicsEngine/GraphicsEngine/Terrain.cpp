#include "stdafx.h"
#include "Terrain.h"
#include "Common/Helpers.h"
#include "IScene.h"
#include "TextureManager.h"
#include "Graphics.h"

#include <DirectXPackedVector.h>
#include <algorithm>
#include <random>
#include <cmath>

using namespace Common;
using namespace DirectX;
using namespace GraphicsEngine;
using namespace std;
using namespace Microsoft::WRL;

Terrain::Terrain(const D3DBase& d3dBase, Graphics& graphics, TextureManager& textureManager, IScene& scene, const Description& description) :
	m_description(description)
{
	CreateGeometry(d3dBase, scene);
	CreateMaterial(d3dBase, textureManager, scene);
	CreateRenderItem(d3dBase, graphics, scene);
}

std::vector<DirectX::XMFLOAT3> Terrain::GenerateRandomPositions(SIZE_T count) const
{
	auto halfTerrainWidth = 0.5f * m_description.TerrainWidth;
	auto halfTerrainDepth = 0.5f * m_description.TerrainDepth;

	// Create a uniform distribution [-halfTerrain{Width|Height}, halfTerrain{Width|Height}]:
	auto offsetWidth = halfTerrainWidth * 0.05f;
	auto offsetDepth = halfTerrainDepth * 0.05f;
	std::random_device randomDevice;
	std::default_random_engine randomEngine(randomDevice());
	std::uniform_real_distribution<float> xDistribution(-halfTerrainWidth + offsetWidth, halfTerrainWidth - offsetWidth);
	std::uniform_real_distribution<float> zDistribution(-halfTerrainDepth + offsetDepth, halfTerrainDepth - offsetDepth);

	std::vector<DirectX::XMFLOAT3> randomPositions(count);
	for(SIZE_T i = 0; i < count; ++i)
	{
		auto x = xDistribution(randomEngine);
		auto z = zDistribution(randomEngine);
		auto y = GetTerrainHeight(x, z);

		randomPositions[i] = XMFLOAT3(x, y, z);
	}
		
	return randomPositions;
}

float Terrain::GetTerrainHeight(float x, float z) const
{
	// Clamp out of bounds values:
	auto halfTerrainWidth = 0.5f * m_description.TerrainWidth;
	auto halfTerrainDepth = 0.5f * m_description.TerrainDepth;
	x = min(max(x, -halfTerrainWidth), halfTerrainWidth);
	z = min(max(z, -halfTerrainDepth), halfTerrainDepth);

	// Calculate texture coordinates in the range [0, height_map_{width|height}]:
	auto textureCoordinatesS = static_cast<uint32_t>((x / m_description.TerrainWidth + 0.5f) * m_description.HeightMapWidth);
	auto textureCoordinatesT = static_cast<uint32_t>((-z / m_description.TerrainDepth + 0.5f) * m_description.HeightMapHeight);

	// Get the heights of the cell:
	auto topLeftHeight = m_heightMap[textureCoordinatesT * m_description.HeightMapWidth + textureCoordinatesS];
	auto topRightHeight = m_heightMap[textureCoordinatesT * m_description.HeightMapWidth + textureCoordinatesS + 1];
	auto lowerLeftHeight = m_heightMap[(textureCoordinatesT + 1) * m_description.HeightMapWidth + textureCoordinatesS];
	auto lowerRightHeight = m_heightMap[(textureCoordinatesT + 1) * m_description.HeightMapWidth + textureCoordinatesS + 1];

	// Map from 0.5f * [-terrain_width, terrain_width] to [0, cell_x_count]:
	auto cellRowFloat = (-z / m_description.TerrainDepth + 0.5f) * m_description.CellZCount;
	auto cellColumnFloat = (x / m_description.TerrainWidth + 0.5f) * m_description.CellXCount;

	// Calculate the row and column as integers:
	auto cellRow = static_cast<uint32_t>(floorf(cellRowFloat));
	auto cellColumn = static_cast<uint32_t>(floorf(cellColumnFloat));

	// Calculate position in the range [0, 1] inside the cell:
	auto s = cellColumnFloat - static_cast<float>(cellColumn);
	auto t = cellRowFloat - static_cast<float>(cellRow);

	float height;

	// If the position belongs to the upper triangle:
	if(s + t <= 1.0f)
	{
		auto fromTopLeftToTopRight = topRightHeight - topLeftHeight;
		auto fromTopLeftToLowerLeft = lowerLeftHeight - topLeftHeight;
		height = topLeftHeight + s * fromTopLeftToTopRight + t * fromTopLeftToLowerLeft;
	}
	// If the position belongs to the lower triangle:
	else
	{
		auto fromLowerRightToTopRight = topRightHeight - lowerRightHeight;
		auto fromLowerRightToLowerLeft = lowerLeftHeight - lowerRightHeight;
		height = lowerRightHeight + (1.0f - s) * fromLowerRightToTopRight + (1.0f - t) * fromLowerRightToLowerLeft;
	}

	return height;
}

const Terrain::Description& Terrain::GetDescription() const
{
	return m_description;
}

DirectX::XMFLOAT2 Terrain::GetTexelSize() const
{
	return XMFLOAT2(1.0f / static_cast<float>(m_description.HeightMapWidth), 1.0f / static_cast<float>(m_description.HeightMapHeight));
}

void Terrain::CreateGeometry(const D3DBase& d3dBase, IScene& scene) const
{
	auto device = d3dBase.GetDevice();

	auto meshData = CreateMeshData(m_description.TerrainWidth, m_description.TerrainDepth, m_description.CellXCount, m_description.CellZCount);

	auto terrainGeometry = std::make_unique<MeshGeometry>();
	terrainGeometry->Name = "TerrainGeometry";
	terrainGeometry->Vertices = VertexBuffer(device, meshData.Vertices);
	terrainGeometry->Indices = IndexBuffer(device, meshData.Indices);

	// Submesh:
	SubmeshGeometry terrainSubmesh;
	terrainSubmesh.IndexCount = static_cast<uint32_t>(meshData.Indices.size());
	terrainSubmesh.StartIndexLocation = 0;
	terrainSubmesh.BaseVertexLocation = 0;
	terrainSubmesh.Bounds = MeshGeometry::CreateBoundingBoxFromMesh(meshData.Vertices);
	terrainGeometry->Submeshes["TerrainSubmesh"] = std::move(terrainSubmesh);

	scene.AddGeometry(std::move(terrainGeometry));
}
void Terrain::CreateMaterial(const D3DBase& d3dBase, TextureManager& textureManager, IScene& scene)
{
	auto device = d3dBase.GetDevice();

	auto material = std::make_unique<Material>();
	material->Name = "TerrainMaterial";

	// Load textures:
	{
		textureManager.Create(device, "TerrainTiledDiffuseMap", m_description.TiledDiffuseMapFilename);
		material->DiffuseMap = &textureManager["TerrainTiledDiffuseMap"];

		textureManager.Create(device, "TerrainTiledNormalMap", m_description.TiledNormalMapFilename);
		material->NormalMap = &textureManager["TerrainTiledNormalMap"];

		// Load height map:
		{
			D3D11_TEXTURE2D_DESC heightMapDescription;
			heightMapDescription.Width = m_description.HeightMapWidth;
			heightMapDescription.Height = m_description.HeightMapHeight;
			heightMapDescription.MipLevels = 1;
			heightMapDescription.ArraySize = 1;
			heightMapDescription.Format = DXGI_FORMAT_R16_FLOAT;
			heightMapDescription.SampleDesc.Count = 1;
			heightMapDescription.SampleDesc.Quality = 0;
			heightMapDescription.Usage = D3D11_USAGE_DEFAULT;
			heightMapDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			heightMapDescription.CPUAccessFlags = 0;
			heightMapDescription.MiscFlags = 0;

			// Load height map:
			LoadRawHeightMap(m_description.HeightMapFilename, m_description.HeightMapWidth, m_description.HeightMapHeight, m_description.HeightMapFactor, m_heightMap);

			// Convert from vector of floats to vector of halfs:
			std::vector<PackedVector::HALF> heightMapHalf(m_heightMap.size());
			std::transform(m_heightMap.begin(), m_heightMap.end(), heightMapHalf.begin(), PackedVector::XMConvertFloatToHalf);

			// Create texture:
			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = &heightMapHalf[0];
			data.SysMemPitch = static_cast<UINT>(m_description.HeightMapWidth * sizeof(PackedVector::HALF));
			data.SysMemSlicePitch = 0;

			ComPtr<ID3D11Texture2D> heightMapTexture;
			ThrowIfFailed(device->CreateTexture2D(&heightMapDescription, &data, heightMapTexture.GetAddressOf()));

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDescription;
			srvDescription.Format = heightMapDescription.Format;
			srvDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDescription.Texture2D.MostDetailedMip = 0;
			srvDescription.Texture2D.MipLevels = -1;

			textureManager.Add("TerrainHeightMap", Texture(device, "TerrainHeightMap", heightMapTexture.Get(), srvDescription));
			material->HeightMap = &textureManager["TerrainHeightMap"];
		}
	}

	// Material parameters:
	material->DiffuseAlbedo = { 0.8f, 0.8f, 0.8f, 1.0f };
	material->FresnelR0 = { 0.01f, 0.01f, 0.01f };
	material->Roughness = 0.25f;
	material->MaterialTransform = MathHelper::Identity4x4();

	scene.AddMaterial(std::move(material));
}
void Terrain::CreateRenderItem(const D3DBase& d3dBase, Graphics& graphics, IScene& scene)
{
	auto renderItem = std::make_unique<RenderItem>();
	renderItem->Name = "Terrain";
	renderItem->Mesh = scene.GetGeometries().at("TerrainGeometry").get();
	renderItem->Material = scene.GetMaterials().at("TerrainMaterial").get();
	renderItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;

	const auto& terrainSubmesh = renderItem->Mesh->Submeshes.at("TerrainSubmesh");
	renderItem->IndexCount = terrainSubmesh.IndexCount;
	renderItem->StartIndexLocation = terrainSubmesh.StartIndexLocation;
	renderItem->BaseVertexLocation = terrainSubmesh.BaseVertexLocation;
	renderItem->Bounds = terrainSubmesh.Bounds;

	// Add instance:
	{
		ShaderBufferTypes::InstanceData instanceData;
		instanceData.WorldMatrix = MathHelper::Identity4x4();
		renderItem->InstancesData.push_back(std::move(instanceData));
	}

	graphics.AddRenderItem(std::move(renderItem), { RenderLayer::Terrain });
}

GeometryGenerator2::MeshData Terrain::CreateMeshData(float width, float depth, uint32_t xCellCount, uint32_t zCellCount)
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
void Terrain::LoadRawHeightMap(const std::wstring& filename, uint32_t width, uint32_t height, float heightFactor, std::vector<float>& heightMap)
{
	// Read file content into a buffer:
	std::vector<uint16_t> buffer;
	Helpers::ReadData(filename, buffer);

	if (width * height != buffer.size())
		ThrowEngineException(L"Terrain dimensions don't match with the height map dimensions!");

	heightMap.resize(static_cast<SIZE_T>(width * height));
	for (uint32_t i = 0; i < width; ++i)
	{
		for (uint32_t j = 0; j < height; ++j)
		{
			auto index = i * width + j;

			//heightMap[index] = heightFactor * static_cast<float>(buffer[index]) / 65535.0f;
			heightMap[index] = heightFactor * 0.0f;
		}
	}
}
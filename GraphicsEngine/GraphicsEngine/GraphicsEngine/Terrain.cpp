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
#include "ImmutableMeshGeometry.h"
#include "NormalRenderItem.h"

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
	for (SIZE_T i = 0; i < count; ++i)
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
	if (s + t <= 1.0f)
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

DirectX::XMFLOAT3 Terrain::TextureSpaceToWorldSpace(const DirectX::XMFLOAT2& position) const
{
	auto x = position.x - m_description.HeightMapWidth / 2.0f;
	auto z = -(position.y - m_description.HeightMapHeight / 2.0f);
	auto y = GetTerrainHeight(x, z);
	return XMFLOAT3(x, y, z);
}

void Terrain::SetMeshData(std::vector<VertexTypes::PositionVertexType>&& vertices, std::vector<uint32_t>&& indices)
{
	m_vertices = std::move(vertices);
	m_indices = std::move(indices);
}

void Terrain::GetMeshData(const std::vector<VertexTypes::PositionVertexType>*& vertices, const std::vector<uint32_t>*& indices) const
{
	vertices = &m_vertices;
	indices = &m_indices;
}

void Terrain::CreateGeometry(const D3DBase& d3dBase, IScene& scene) const
{
	auto device = d3dBase.GetDevice();

	auto meshData = CreateMeshData(m_description.TerrainWidth, m_description.TerrainDepth, m_description.CellXCount, m_description.CellZCount);

	auto terrainGeometry = std::make_unique<ImmutableMeshGeometry>();
	terrainGeometry->SetName("TerrainGeometry");
	terrainGeometry->CreateVertexBuffer(device, meshData.Vertices, D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	terrainGeometry->CreateIndexBuffer(device, meshData.Indices, DXGI_FORMAT_R32_UINT);

	// Submesh:
	SubmeshGeometry terrainSubmesh;
	terrainSubmesh.IndexCount = static_cast<uint32_t>(meshData.Indices.size());
	terrainSubmesh.StartIndexLocation = 0;
	terrainSubmesh.BaseVertexLocation = 0;
	terrainSubmesh.Bounds = BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(m_description.TerrainWidth, m_description.HeightMapFactor * 2.0f, m_description.TerrainDepth));
	terrainGeometry->AddSubmesh("TerrainSubmesh", std::move(terrainSubmesh));

	scene.AddGeometry(std::move(terrainGeometry));
}
void Terrain::CreateMaterial(const D3DBase& d3dBase, TextureManager& textureManager, IScene& scene)
{
	auto device = d3dBase.GetDevice();

	auto material = std::make_unique<Material>();
	material->Name = "TerrainMaterial";

	// Load textures:
	{
		// Load tiled textures:
		for(const auto& texturesFilenames : m_description.TiledTexturesFilenames)
		{
			TextureArray textureArray;

			for(const auto& pair : texturesFilenames)
			{
				const auto& textureName = pair.first;
				const auto& textureFilename = pair.second;
				textureManager.Create(device, textureName, textureFilename);
				textureArray.Add(textureManager[textureName].Get());
			}

			material->TiledMapsArrays.push_back(std::move(textureArray));
		}

		// Load height map:
		{
			// Load height map:
			auto width = m_description.HeightMapWidth;
			auto height = m_description.HeightMapHeight;
			LoadRawHeightMap(m_description.HeightMapFilename, width, height, m_description.HeightMapFactor, m_heightMap, m_normalMap, m_tangentMap);

			// Create height map texture:
			{
				// Convert from vector of floats to vector of halfs:
				std::vector<PackedVector::HALF> heightMapHalf(m_heightMap.size());
				std::transform(m_heightMap.begin(), m_heightMap.end(), heightMapHalf.begin(), PackedVector::XMConvertFloatToHalf);

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

			// Normal map:
			{
				// Convert from vector of floats to vector of halfs:
				std::vector<PackedVector::XMHALF4> normalMapHalf(m_normalMap.size());
				std::transform(m_normalMap.begin(), m_normalMap.end(), normalMapHalf.begin(), MathHelper::ConvertFloat4ToHalf4);

				D3D11_TEXTURE2D_DESC normalMapDescription;
				normalMapDescription.Width = m_description.HeightMapWidth;
				normalMapDescription.Height = m_description.HeightMapHeight;
				normalMapDescription.MipLevels = 1;
				normalMapDescription.ArraySize = 1;
				normalMapDescription.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
				normalMapDescription.SampleDesc.Count = 1;
				normalMapDescription.SampleDesc.Quality = 0;
				normalMapDescription.Usage = D3D11_USAGE_DEFAULT;
				normalMapDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				normalMapDescription.CPUAccessFlags = 0;
				normalMapDescription.MiscFlags = 0;

				D3D11_SUBRESOURCE_DATA data;
				data.pSysMem = &normalMapHalf[0];
				data.SysMemPitch = static_cast<UINT>(m_description.HeightMapWidth * sizeof(PackedVector::XMHALF4));
				data.SysMemSlicePitch = 0;

				ComPtr<ID3D11Texture2D> normalMapTexture;
				ThrowIfFailed(device->CreateTexture2D(&normalMapDescription, &data, normalMapTexture.GetAddressOf()));

				D3D11_SHADER_RESOURCE_VIEW_DESC srvDescription;
				srvDescription.Format = normalMapDescription.Format;
				srvDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDescription.Texture2D.MostDetailedMip = 0;
				srvDescription.Texture2D.MipLevels = -1;

				auto normalMapTextureName = "TerrainNormalMap";
				textureManager.Add(normalMapTextureName, Texture(device, normalMapTextureName, normalMapTexture.Get(), srvDescription));
				material->NormalMap = &textureManager[normalMapTextureName];
			}

			// Tangent map:
			{
				// Convert from vector of floats to vector of halfs:
				std::vector<PackedVector::XMHALF4> tangentMapHalf(m_tangentMap.size());
				std::transform(m_tangentMap.begin(), m_tangentMap.end(), tangentMapHalf.begin(), MathHelper::ConvertFloat4ToHalf4);

				D3D11_TEXTURE2D_DESC tangentMapDescription;
				tangentMapDescription.Width = m_description.HeightMapWidth;
				tangentMapDescription.Height = m_description.HeightMapHeight;
				tangentMapDescription.MipLevels = 1;
				tangentMapDescription.ArraySize = 1;
				tangentMapDescription.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
				tangentMapDescription.SampleDesc.Count = 1;
				tangentMapDescription.SampleDesc.Quality = 0;
				tangentMapDescription.Usage = D3D11_USAGE_DEFAULT;
				tangentMapDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				tangentMapDescription.CPUAccessFlags = 0;
				tangentMapDescription.MiscFlags = 0;

				D3D11_SUBRESOURCE_DATA data;
				data.pSysMem = &tangentMapHalf[0];
				data.SysMemPitch = static_cast<UINT>(m_description.HeightMapWidth * sizeof(PackedVector::XMHALF4));
				data.SysMemSlicePitch = 0;

				ComPtr<ID3D11Texture2D> tangentMapTexture;
				ThrowIfFailed(device->CreateTexture2D(&tangentMapDescription, &data, tangentMapTexture.GetAddressOf()));

				D3D11_SHADER_RESOURCE_VIEW_DESC srvDescription;
				srvDescription.Format = tangentMapDescription.Format;
				srvDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDescription.Texture2D.MostDetailedMip = 0;
				srvDescription.Texture2D.MipLevels = -1;

				auto tangentMapTextureName = "TerrainTangentMap";
				textureManager.Add(tangentMapTextureName, Texture(device, tangentMapTextureName, tangentMapTexture.Get(), srvDescription));
				material->TangentMap = &textureManager[tangentMapTextureName];
			}
		}
	}

	// Material parameters:
	material->DiffuseAlbedo = { 0.8f, 0.8f, 0.8f, 1.0f };
	material->FresnelR0 = { 0.01f, 0.01f, 0.01f };
	material->Roughness = 0.25f;
	material->MaterialTransform = MathHelper::Identity4x4();

	scene.AddMaterial(std::move(material));
}
void Terrain::CreateRenderItem(const D3DBase& d3dBase, Graphics& graphics, IScene& scene) const
{
	auto renderItem = std::make_unique<NormalRenderItem>();
	renderItem->SetName("Terrain");
	renderItem->SetMesh(dynamic_cast<ImmutableMeshGeometry*>(scene.GetGeometries().at("TerrainGeometry").get()), "TerrainSubmesh");
	renderItem->SetMaterial(scene.GetMaterials().at("TerrainMaterial").get());

	graphics.AddNormalRenderItem(std::move(renderItem), { RenderLayer::Terrain });
}

GeometryGenerator::MeshData Terrain::CreateMeshData(float width, float depth, uint32_t xCellCount, uint32_t zCellCount)
{
	GeometryGenerator::MeshData output;

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
void Terrain::LoadRawHeightMap(const std::wstring& filename, uint32_t width, uint32_t height, float heightFactor, std::vector<float>& heightMap, std::vector<XMFLOAT4>& normalMap, std::vector<XMFLOAT4>& tangentMap)
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

			heightMap[index] = heightFactor * static_cast<float>(buffer[index]) / 65535.0f;
			//heightMap[index] = heightFactor * 0.0f;
		}
	}

	// Compute the tangent, bitangent and normal vectors.
	// position = (x, f(x, z), z)
	// tangent = d(position) / dx = (1.0f, d(f(x, z)) / dx, 0.0f)
	// bitangent = d(position) / dz = (0.0f, d(f(x, z)) / dz, 1.0f)
	// Calculate the derivative using the central differences method, with h = 2.
	normalMap.resize(static_cast<SIZE_T>(heightMap.size()));
	tangentMap.resize(static_cast<SIZE_T>(heightMap.size()));
	for (uint32_t i = 0; i < width; ++i)
	{
		for (uint32_t j = 0; j < height; ++j)
		{
			auto iDown = i == height - 1 ? 0 : i + 1;
			auto iUp = i == 0 ? height - 1 : i - 1;
			auto jLeft = j == 0 ? width - 1 : j - 1;
			auto jRight = j == width - 1 ? 0 : j + 1;

			auto downValue = heightMap[iDown * width + j];
			auto upValue = heightMap[iUp * width + j];
			auto leftValue = heightMap[i * width + jLeft];
			auto rightValue = heightMap[i * width + jRight];

			auto tangentVector = XMVector3Normalize(XMVectorSet(2.0f, (upValue - downValue), 0.0f, 0.0f));
			auto bitangentVector = XMVector3Normalize(XMVectorSet(0.0f, (rightValue - leftValue), -2.0f, 0.0f));
			auto normalVector = XMVector3Cross(tangentVector, bitangentVector);

			auto index = i * width + j;
			XMStoreFloat4(&tangentMap[index], tangentVector);
			XMStoreFloat4(&normalMap[index], normalVector);
		}
	}
}

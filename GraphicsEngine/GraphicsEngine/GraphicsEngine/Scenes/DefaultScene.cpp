#include "stdafx.h"
#include "DefaultScene.h"
#include "GraphicsEngine/Graphics.h"
#include "GraphicsEngine/GeometryGenerator.h"
#include "GraphicsEngine/VertexTypes.h"
#include "GraphicsEngine/RenderItem.h"
#include "GraphicsEngine/AssimpImporter/AssimpImporter.h"
#include "GraphicsEngine/TerrainBuilder.h"
#include "Common/Helpers.h"

using namespace DirectX;
using namespace Common;
using namespace GraphicsEngine;

DefaultScene::DefaultScene(Graphics* graphics, const D3DBase& d3dBase, TextureManager& textureManager)
{
	InitializeGeometry(d3dBase);
	InitializeTextures(d3dBase, textureManager);
	InitializeMaterials(textureManager);
	InitializeRenderItems(graphics);

	InitializeExternalModels(graphics, d3dBase, textureManager);
	InitializeTerrain(graphics, d3dBase, textureManager);
}

void DefaultScene::AddGeometry(std::unique_ptr<MeshGeometry>&& geometry)
{
	m_geometries.emplace(geometry->Name, std::move(geometry));
}
void DefaultScene::AddMaterial(std::unique_ptr<Material>&& material)
{
	material->MaterialIndex = static_cast<int>(m_materials.size());
	m_materials.emplace(material->Name, std::move(material));
}

const std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& DefaultScene::GetGeometries() const
{
	return m_geometries;
}
const std::unordered_map<std::string, std::unique_ptr<Material>>& DefaultScene::GetMaterials() const
{
	return m_materials;
}

void DefaultScene::InitializeGeometry(const D3DBase& d3dBase)
{
	// ShapeGeo:
	{
		auto box = GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f, 3);
		auto grid = GeometryGenerator::CreateGrid(20.0f, 30.0f, 60, 40);
		auto sphere = GeometryGenerator::CreateSphere(0.5f, 20, 20);
		auto cylinder = GeometryGenerator::CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

		//
		// We are concatenating all the geometry into one big vertex/index buffer.  So
		// define the regions in the buffer each submesh covers.
		//

		// Cache the vertex offsets to each object in the concatenated vertex buffer.
		UINT boxVertexOffset = 0;
		UINT gridVertexOffset = static_cast<UINT>(box.Vertices.size());
		UINT sphereVertexOffset = gridVertexOffset + static_cast<UINT>(grid.Vertices.size());
		UINT cylinderVertexOffset = sphereVertexOffset + static_cast<UINT>(sphere.Vertices.size());

		// Cache the starting index for each object in the concatenated index buffer.
		UINT boxIndexOffset = 0;
		UINT gridIndexOffset = static_cast<UINT>(box.Indices32.size());
		UINT sphereIndexOffset = gridIndexOffset + static_cast<UINT>(grid.Indices32.size());
		UINT cylinderIndexOffset = sphereIndexOffset + static_cast<UINT>(sphere.Indices32.size());

		SubmeshGeometry boxSubmesh;
		boxSubmesh.IndexCount = static_cast<UINT>(box.Indices32.size());
		boxSubmesh.StartIndexLocation = boxIndexOffset;
		boxSubmesh.BaseVertexLocation = boxVertexOffset;
		boxSubmesh.Bounds = MeshGeometry::CreateBoundingBoxFromMesh(box);

		SubmeshGeometry gridSubmesh;
		gridSubmesh.IndexCount = static_cast<UINT>(grid.Indices32.size());
		gridSubmesh.StartIndexLocation = gridIndexOffset;
		gridSubmesh.BaseVertexLocation = gridVertexOffset;
		gridSubmesh.Bounds = MeshGeometry::CreateBoundingBoxFromMesh(grid);

		SubmeshGeometry sphereSubmesh;
		sphereSubmesh.IndexCount = static_cast<UINT>(sphere.Indices32.size());
		sphereSubmesh.StartIndexLocation = sphereIndexOffset;
		sphereSubmesh.BaseVertexLocation = sphereVertexOffset;
		sphereSubmesh.Bounds = MeshGeometry::CreateBoundingBoxFromMesh(sphere);

		SubmeshGeometry cylinderSubmesh;
		cylinderSubmesh.IndexCount = static_cast<UINT>(cylinder.Indices32.size());
		cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
		cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;
		cylinderSubmesh.Bounds = MeshGeometry::CreateBoundingBoxFromMesh(cylinder);

		//
		// Extract the vertex elements we are interested in and pack the
		// vertices of all the meshes into one vertex buffer.
		//

		auto totalVertexCount =
			box.Vertices.size() +
			grid.Vertices.size() +
			sphere.Vertices.size() +
			cylinder.Vertices.size();

		std::vector<VertexTypes::DefaultVertexType> vertices(totalVertexCount);

		UINT k = 0;
		for (SIZE_T i = 0; i < box.Vertices.size(); ++i, ++k)
		{
			const auto& vertex = box.Vertices[i];
			vertices[k].Position = vertex.Position;
			vertices[k].Normal = vertex.Normal;
			vertices[k].TextureCoordinates = vertex.TextureCoordinates;
		}

		for (SIZE_T i = 0; i < grid.Vertices.size(); ++i, ++k)
		{
			const auto& vertex = grid.Vertices[i];
			vertices[k].Position = vertex.Position;
			vertices[k].Normal = vertex.Normal;
			vertices[k].TextureCoordinates = vertex.TextureCoordinates;
		}

		for (SIZE_T i = 0; i < sphere.Vertices.size(); ++i, ++k)
		{
			const auto& vertex = sphere.Vertices[i];
			vertices[k].Position = vertex.Position;
			vertices[k].Normal = vertex.Normal;
			vertices[k].TextureCoordinates = vertex.TextureCoordinates;
		}

		for (SIZE_T i = 0; i < cylinder.Vertices.size(); ++i, ++k)
		{
			const auto& vertex = cylinder.Vertices[i];
			vertices[k].Position = vertex.Position;
			vertices[k].Normal = vertex.Normal;
			vertices[k].TextureCoordinates = vertex.TextureCoordinates;
		}

		std::vector<std::uint16_t> indices;
		indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
		indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
		indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
		indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

		auto device = d3dBase.GetDevice();

		auto geo = std::make_unique<MeshGeometry>();
		geo->Name = "ShapeGeo";
		geo->Vertices = VertexBuffer(device, vertices);
		geo->Indices = IndexBuffer(device, indices);
		geo->Submeshes["Box"] = boxSubmesh;
		geo->Submeshes["Grid"] = gridSubmesh;
		geo->Submeshes["Sphere"] = sphereSubmesh;
		geo->Submeshes["Cylinder"] = cylinderSubmesh;

		m_geometries[geo->Name] = std::move(geo);
	}
}
void DefaultScene::InitializeTextures(const D3DBase& d3dBase, TextureManager& textureManager)
{
	auto device = d3dBase.GetDevice();

	textureManager.Create(device, "BricksTexture", L"Textures/Started01.dds");
}
void DefaultScene::InitializeMaterials(TextureManager& textureManager)
{
	auto bricks = std::make_unique<Material>();
	bricks->Name = "Bricks";
	bricks->DiffuseMap = &textureManager["BricksTexture"];
	bricks->MaterialIndex = 0;
	bricks->DiffuseAlbedo = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	bricks->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	bricks->Roughness = 0.25f;
	bricks->MaterialTransform = MathHelper::Identity4x4();
	AddMaterial(std::move(bricks));
}

void DefaultScene::InitializeRenderItems(Graphics* graphics)
{
	// Box:
	{
		auto boxRenderItem = std::make_unique<RenderItem>();
		boxRenderItem->Name = "Box";
		boxRenderItem->Mesh = m_geometries["ShapeGeo"].get();
		boxRenderItem->Material = m_materials.at("Bricks").get();
		boxRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		const auto& boxSubmesh = boxRenderItem->Mesh->Submeshes.at("Box");
		boxRenderItem->IndexCount = boxSubmesh.IndexCount;
		boxRenderItem->StartIndexLocation = boxSubmesh.StartIndexLocation;
		boxRenderItem->BaseVertexLocation = boxSubmesh.BaseVertexLocation;
		boxRenderItem->Bounds = boxSubmesh.Bounds;

		// Instances:
		/*{
			const auto size = 3;
			const auto offset = 2.0f;
			const auto start = -size  * offset / 2.0f;
			boxRenderItem->InstancesData.reserve(size * size * size);
			for (SIZE_T i = 0; i < size; ++i)
			{
				for (SIZE_T j = 0; j < size; ++j)
				{
					for (SIZE_T k = 0; k < size; ++k)
					{
						ShaderBufferTypes::InstanceData instanceData;

						XMStoreFloat4x4(&instanceData.WorldMatrix, XMMatrixTranslation(start + i * offset, start + j * offset, start + k * offset));

						boxRenderItem->InstancesData.push_back(instanceData);
					}
				}
			}
		}

		graphics->AddRenderItem(std::move(boxRenderItem), { RenderLayer::Opaque });*/
	}

	// Sphere:
/*	{
		auto sphereRenderItem = std::make_unique<RenderItem>();
		sphereRenderItem->Mesh = m_geometries["ShapeGeo"].get();
		sphereRenderItem->Material = m_materials["Bricks"].get();
		sphereRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		sphereRenderItem->IndexCount = sphereRenderItem->Mesh->Submeshes.at("Sphere").IndexCount;
		sphereRenderItem->StartIndexLocation = sphereRenderItem->Mesh->Submeshes.at("Sphere").StartIndexLocation;
		sphereRenderItem->BaseVertexLocation = sphereRenderItem->Mesh->Submeshes.at("Sphere").BaseVertexLocation;

		sphereRenderItem->ObjectBufferIndex = 1;
		XMStoreFloat4x4(&sphereRenderItem->WorldMatrix, XMMatrixTranslation(0.0f, 0.0f, -5.0f));

		graphics->AddRenderItem(std::move(sphereRenderItem), { RenderLayer::Transparent });
	}*/
}

void DefaultScene::InitializeExternalModels(Graphics* graphics, const D3DBase& d3dBase, TextureManager& textureManager)
{

	AssimpImporter importer;
	AssimpImporter::ImportInfo importInfo;
	std::wstring filename(L"Models/Cube.fbx");
	importer.Import(graphics, d3dBase, textureManager, this, filename, importInfo);

	auto importedGeometry = m_geometries.at(Helpers::WStringToString(filename)).get();
	const auto& submesh = importedGeometry->Submeshes.at("Cube");
	const auto& materialName = importInfo.MaterialByMesh.at(AssimpImporter::BuildMeshName(filename, "Cube"));

	auto cubeRenderItem = std::make_unique<RenderItem>();
	cubeRenderItem->Name = "Cube";
	cubeRenderItem->Mesh = importedGeometry;
	cubeRenderItem->Material = m_materials.at(materialName).get();
	cubeRenderItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	cubeRenderItem->IndexCount = submesh.IndexCount;
	cubeRenderItem->StartIndexLocation = submesh.StartIndexLocation;
	cubeRenderItem->BaseVertexLocation = submesh.BaseVertexLocation;
	cubeRenderItem->Bounds = submesh.Bounds;



	// Instances:
	const auto size = 10;
	const auto offset = 4.0f;
	const auto start = -size  * offset / 2.0f;
	cubeRenderItem->InstancesData.reserve(size * size * size);
	for (SIZE_T i = 0; i < size; ++i)
	{
		for (SIZE_T j = 0; j < size; ++j)
		{
			for (SIZE_T k = 0; k < size; ++k)
			{
				ShaderBufferTypes::InstanceData instanceData;

				XMStoreFloat4x4(&instanceData.WorldMatrix, XMMatrixTranslation(start + i * offset, start + j * offset, start + k * offset));

				cubeRenderItem->InstancesData.push_back(instanceData);
			}
		}
	}

	graphics->AddRenderItem(std::move(cubeRenderItem), { RenderLayer::Opaque });

	// Sky Dome model init
	std::wstring domefilename(L"Models/SkyDome.fbx");
	importer.Import(graphics, d3dBase, textureManager, this, domefilename, importInfo);

	auto domeGeometry = m_geometries.at(Helpers::WStringToString(domefilename)).get();
	const auto& domeSubmesh = domeGeometry->Submeshes.at("Sphere");
	
	auto domeMaterial = std::make_unique<Material>();
	domeMaterial->Name = "DomeMaterial";
	AddMaterial(std::move(domeMaterial));

	auto domeRenderItem = std::make_unique<RenderItem>();
	domeRenderItem->Name = "SkyDome";
	domeRenderItem->Mesh = domeGeometry;
	domeRenderItem->Material = m_materials.at("DomeMaterial").get();
	domeRenderItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	domeRenderItem->IndexCount = domeSubmesh.IndexCount;
	domeRenderItem->StartIndexLocation = domeSubmesh.StartIndexLocation;
	domeRenderItem->BaseVertexLocation = domeSubmesh.BaseVertexLocation;
	domeRenderItem->Bounds = domeSubmesh.Bounds;

	ShaderBufferTypes::InstanceData instanceData;
	XMStoreFloat4x4(&instanceData.WorldMatrix, XMMatrixScaling(100,100,100));
	domeRenderItem->InstancesData.push_back(instanceData);

	graphics->AddRenderItem(std::move(domeRenderItem), { RenderLayer::SkyDome});

}

void DefaultScene::InitializeTerrain(Graphics* graphics, const D3DBase& d3dBase, TextureManager& textureManager)
{
	auto device = d3dBase.GetDevice();

	auto meshData = TerrainBuilder::CreateTerrain(
		TerrainBuilder::s_defaultTerrainWidth,
		TerrainBuilder::s_defaultTerrainDepth,
		TerrainBuilder::s_defaultTerrainXCellCount,
		TerrainBuilder::s_defaultTerrainZCellCount
	);

	// Create geometry:
	{
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

		AddGeometry(std::move(terrainGeometry));
	}

	// Create material:
	{
		auto terrainMaterial = std::make_unique<Material>();
		terrainMaterial->Name = "TerrainMaterial";

		// Add textures:
		{
			textureManager.Create(device, "TerrainDiffuseMap", L"Textures/TerrainDiffuseMap.dds");
			terrainMaterial->DiffuseMap = &textureManager["TerrainDiffuseMap"];

			textureManager.Create(device, "TerrainNormalMap", L"Textures/TerrainNormalMap.dds");
			terrainMaterial->NormalMap = &textureManager["TerrainNormalMap"];

			textureManager.Create(device, "TerrainHeightMap", L"Textures/terrain_height_map.dds");
			terrainMaterial->HeightMap = &textureManager["TerrainHeightMap"];
		}

		// Material parameters:
		terrainMaterial->DiffuseAlbedo = { 0.8f, 0.8f, 0.8f, 1.0f };
		terrainMaterial->FresnelR0 = { 0.01f, 0.01f, 0.01f };
		terrainMaterial->Roughness = 0.25f;
		terrainMaterial->MaterialTransform = MathHelper::Identity4x4();

		AddMaterial(std::move(terrainMaterial));
	}

	auto renderItem = std::make_unique<RenderItem>();
	renderItem->Name = "Terrain";
	renderItem->Mesh = m_geometries.at("TerrainGeometry").get();
	renderItem->Material = m_materials.at("TerrainMaterial").get();
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

	graphics->AddRenderItem(std::move(renderItem), { RenderLayer::Terrain });
}

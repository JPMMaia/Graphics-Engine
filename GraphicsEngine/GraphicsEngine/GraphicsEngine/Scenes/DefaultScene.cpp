#include "stdafx.h"
#include "DefaultScene.h"
#include "GraphicsEngine/Graphics.h"
#include "GraphicsEngine/VertexTypes.h"
#include "GraphicsEngine/RenderItem.h"
#include "GraphicsEngine/AssimpImporter/AssimpImporter.h"
#include "GraphicsEngine/Terrain.h"
#include "GraphicsEngine/GeometryGenerator.h"
#include "Common/Helpers.h"

#include <numeric>
#include "GraphicsEngine/ImmutableMeshGeometry.h"

using namespace DirectX;
using namespace Common;
using namespace GraphicsEngine;

DefaultScene::DefaultScene(Graphics* graphics, const D3DBase& d3dBase, TextureManager& textureManager, LightManager& lightManager) :
	m_initialized(false),
	m_grassRotation(0.0f),
	m_windDirection(1.0f),
	m_sceneBuilder(L"Instances.json")
{
	InitializeTerrain(graphics, d3dBase, textureManager);
	InitializeGeometry(d3dBase);
	InitializeTextures(d3dBase, textureManager);
	InitializeMaterials(textureManager);
	InitializeRenderItems(graphics, d3dBase, textureManager);
	InitializeLights(lightManager);

	m_initialized = true;
}

void DefaultScene::Update(const Graphics& graphics, const Common::Timer& timer)
{
	auto deltaSeconds = static_cast<float>(timer.GetMillisecondsPerUpdate()) / 1000.0f;

	const float limit = XM_PI / 32.0f;
	if (m_grassRotation >= limit)
		m_windDirection = -1.0f;
	else if (m_grassRotation <= -limit)
		m_windDirection = 1.0f;
	m_grassRotation += m_windDirection * deltaSeconds * XM_PI / 32.0f;

	auto grassTransformMatrix = XMMatrixTranslation(-0.5f, -0.5f, -0.5f);
	grassTransformMatrix *= XMMatrixRotationZ(m_grassRotation);
	grassTransformMatrix *= XMMatrixTranslation(0.5f, 0.5f, 0.5f);
	XMStoreFloat4x4(&m_grassTransformMatrix, grassTransformMatrix);
}

void DefaultScene::AddGeometry(std::unique_ptr<MeshGeometry>&& geometry)
{
	m_geometries.emplace(geometry->GetName(), std::move(geometry));
}
void DefaultScene::AddMaterial(std::unique_ptr<Material>&& material)
{
	material->MaterialIndex = static_cast<int>(m_materials.size());
	m_materials.emplace(material->Name, std::move(material));
}

const Terrain& DefaultScene::GetTerrain() const
{
	return m_terrain;
}
Terrain& DefaultScene::GetTerrain()
{
	return m_terrain;
}
const DirectX::XMFLOAT4X4& DefaultScene::GetGrassTransformMatrix() const
{
	return m_grassTransformMatrix;
}
const std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& DefaultScene::GetGeometries() const
{
	return m_geometries;
}
const std::unordered_map<std::string, std::unique_ptr<Material>>& DefaultScene::GetMaterials() const
{
	return m_materials;
}

void DefaultScene::AddInstances(Graphics* graphics, std::string name, const std::initializer_list<std::string>& renderItemNames, const std::vector<SceneBuilder::RenderItemInstanceData>& instancesData, FXMMATRIX transformMatrix)
{
	std::vector<RenderItem*> renderItems;
	renderItems.reserve(renderItemNames.size());
	for(const auto& renderItemName : renderItemNames)
	{
		auto renderItem = graphics->GetRenderItem(renderItemName)->get();
		renderItem->InstancesData.reserve(renderItem->InstancesData.capacity() + instancesData.size());

		renderItems.push_back(renderItem);
	}

	auto instanceDataCount = instancesData.size();
	for (size_t i = 0; i < instanceDataCount; ++i)
	{
		const auto& instanceData = instancesData[i];
		
		const auto& position = instanceData.Position;
		XMFLOAT3 positionW(position.x, m_terrain.GetTerrainHeight(position.x, position.y), position.y);

		ShaderBufferTypes::InstanceData instanceDataBuffer;

		auto scaleMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&instanceData.Scale));
		auto rotationMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&instanceData.Rotation));
		auto translationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&positionW));
		XMStoreFloat4x4(&instanceDataBuffer.WorldMatrix, transformMatrix * scaleMatrix * rotationMatrix * translationMatrix);

		for (auto renderItem : renderItems)
			graphics->AddRenderItemInstance(renderItem, instanceDataBuffer);

		if(m_initialized)
		{
			m_sceneBuilder.AddRenderItemInstance(name, instanceData);
		}
	}
}
void DefaultScene::AddTreeInstances(Graphics* graphics, const std::vector<SceneBuilder::RenderItemInstanceData>& instancesData)
{
	auto scale = 0.3f;
	auto transformationMatrix = XMMatrixScaling(scale, scale, scale) * XMMatrixRotationX(XM_PI / 2.0f);

	AddInstances(graphics, "Tree", { "Trunk", "Leaves" }, instancesData, transformationMatrix);
}
void DefaultScene::RemoveLastInstance(Graphics* graphics, const std::string& itemName, const std::initializer_list<std::string>& renderItemNames)
{
	for(const auto& renderItemName : renderItemNames)
	{
		auto renderItem = graphics->GetRenderItem(renderItemName)->get();
		renderItem->RemoveLastInstance();
	}

	m_sceneBuilder.RemoveLastRenderItemInstance(itemName);
}

void DefaultScene::InitializeTerrain(Graphics* graphics, const D3DBase& d3dBase, TextureManager& textureManager)
{
	Terrain::Description terrainDescription;
	terrainDescription.TerrainWidth = 1024.0f;
	terrainDescription.TerrainDepth = 1024.0f;
	terrainDescription.CellXCount = 32;
	terrainDescription.CellZCount = 32;

	terrainDescription.TiledTexturesFilenames =
	{
		{
			{ "RockDiffuseMap", L"Textures/rock01d.dds" },
			{ "RockNormalMap", L"Textures/rock01n.png" },
			{ "RockSpecularMap", L"Textures/rock01s.dds" },
		},
		{
			{ "GrassDiffuseMap", L"Textures/ground14d.jpg" },
			{ "GrassNormalMap", L"Textures/ground14n.jpg" },
			{ "GrassSpecularMap", L"Textures/ground14s.jpg" },
		},
		{
			{ "PathDiffuseMap", L"Textures/ground06d.jpg" },
			{ "PathNormalMap", L"Textures/ground06n.jpg" },
			{ "PathSpecularMap", L"Textures/ground06s.jpg" },
			{ "PathAlphaMap", L"Textures/ground06a.dds" },
		},
		{
			{ "SnowNormalMap", L"Textures/snow01n.dds" },
		},
	};

	terrainDescription.HeightMapFilename = L"Textures/TerrainHeightMap.r16";
	terrainDescription.HeightMapWidth = 1024;
	terrainDescription.HeightMapHeight = 1024;
	terrainDescription.HeightMapFactor = 256.0f;
	terrainDescription.TiledTexelScale = 64.0f;
	m_terrain = Terrain(d3dBase, *graphics, textureManager, *this, terrainDescription);
}
void DefaultScene::InitializeGeometry(const D3DBase& d3dBase)
{
	auto device = d3dBase.GetDevice();

	// Billboards:
	{
		std::vector<VertexTypes::BillboardVertexType> vertices =
		{
			{ { 0.0f, 0.0f, 0.0f }, { 10.0f, 10.0f } }
		};
		std::vector<UINT> indices(vertices.size());
		std::iota(indices.begin(), indices.end(), 0);

		auto geometry = std::make_unique<ImmutableMeshGeometry>();
		geometry->SetName("Billboard");
		geometry->CreateVertexBuffer(device, vertices);
		geometry->CreateIndexBuffer(device, indices);
		AddGeometry(std::move(geometry));
	}

	// Grass:
	{
		XMFLOAT2 size = { 1.0f, 1.0f };
		std::vector<VertexTypes::BillboardVertexType> vertices;
		{
			auto randomPositions = m_terrain.GenerateRandomPositions(2000);
			vertices.reserve(randomPositions.size());

			for (const auto& position : randomPositions)
			{
				ShaderBufferTypes::InstanceData instanceData;

				vertices.push_back({ { position.x, position.y + 1.5f, position.z }, size });
			}
		}

		std::vector<UINT> indices(vertices.size());
		std::iota(indices.begin(), indices.end(), 0);

		auto geometry = std::make_unique<ImmutableMeshGeometry>();
		geometry->SetName("Grass01");
		geometry->CreateVertexBuffer(device, vertices);
		geometry->CreateIndexBuffer(device, indices);

		SubmeshGeometry submesh;
		submesh.StartIndexLocation = 0;
		submesh.IndexCount = static_cast<uint32_t>(indices.size());
		submesh.BaseVertexLocation = 0;
		submesh.Bounds = BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(vertices[0].Extents.x, vertices[0].Extents.y, 0.1f));
		geometry->AddSubmesh("Default", std::move(submesh));

		AddGeometry(std::move(geometry));
	}

	// Rectangle for debug:
	{
		auto rectangleMeshData = GeometryGenerator::CreateRectangle(0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

		std::vector<VertexTypes::TextureVertexType> vertices(rectangleMeshData.Vertices.size());
		for (SIZE_T i = 0; i < vertices.size(); ++i)
		{
			auto& vertex = vertices[i];
			vertex.Position = rectangleMeshData.Vertices[i].Position;
			vertex.TextureCoordinates = rectangleMeshData.Vertices[i].TextureCoordinates;
		}

		auto geometry = std::make_unique<ImmutableMeshGeometry>();
		geometry->SetName("Rectangle");
		geometry->CreateVertexBuffer(device, vertices);
		geometry->CreateIndexBuffer(device, rectangleMeshData.Indices);

		SubmeshGeometry submesh;
		submesh.StartIndexLocation = 0;
		submesh.IndexCount = static_cast<uint32_t>(rectangleMeshData.Indices.size());
		submesh.BaseVertexLocation = 0;
		submesh.Bounds = MeshGeometry::CreateBoundingBoxFromMesh(vertices);
		geometry->AddSubmesh("Default", std::move(submesh));

		AddGeometry(std::move(geometry));
	}
}
void DefaultScene::InitializeTextures(const D3DBase& d3dBase, TextureManager& textureManager)
{
	auto device = d3dBase.GetDevice();

	textureManager.Create(device, "BricksTexture", L"Textures/test_diffuse_map.dds");
	textureManager.Create(device, "Grass01d", L"Textures/grass01d.dds");
	textureManager.Create(device, "Test", L"Textures/test_diffuse_map.dds");

	textureManager.Create(device, "GrassDiffuseMap", L"Textures/ground14d.jpg");
	textureManager.Create(device, "GrassNormalMap", L"Textures/ground14n.jpg");
	textureManager.Create(device, "GrassSpecularMap", L"Textures/ground14s.jpg");
}
void DefaultScene::InitializeMaterials(TextureManager& textureManager)
{
	{
		auto bricks = std::make_unique<Material>();
		bricks->Name = "Bricks";
		bricks->DiffuseMap = &textureManager["BricksTexture"];
		bricks->DiffuseAlbedo = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
		bricks->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
		bricks->Roughness = 0.25f;
		bricks->MaterialTransform = MathHelper::Identity4x4();
		AddMaterial(std::move(bricks));
	}

	{
		auto test = std::make_unique<Material>();
		test->Name = "Test";
		test->DiffuseMap = &textureManager["Test"];
		test->DiffuseAlbedo = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
		test->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
		test->Roughness = 0.25f;
		test->MaterialTransform = MathHelper::Identity4x4();
		AddMaterial(std::move(test));
	}

	{
		auto material = std::make_unique<Material>();
		material->Name = "NullTexture";
		material->DiffuseMap = nullptr;
		material->DiffuseAlbedo = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
		material->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
		material->Roughness = 0.25f;
		material->MaterialTransform = MathHelper::Identity4x4();
		AddMaterial(std::move(material));
	}

	{
		auto material = std::make_unique<Material>();
		material->Name = "Grass01";
		material->DiffuseMap = &textureManager["Grass01d"];
		material->DiffuseAlbedo = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		material->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
		material->Roughness = 0.4f;
		material->MaterialTransform = MathHelper::Identity4x4();
		AddMaterial(std::move(material));
	}
}

void DefaultScene::InitializeRenderItems(Graphics* graphics, const D3DBase& d3dBase, TextureManager& textureManager)
{
	// Billboards:
	{
		auto renderItem = std::make_unique<RenderItem>();
		renderItem->Name = "Grass01";
		renderItem->Mesh = m_geometries.at("Billboard").get();
		renderItem->Material = m_materials["Test"].get();
		renderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		renderItem->IndexCount = 1;
		renderItem->StartIndexLocation = 0;
		renderItem->BaseVertexLocation = 0;
		renderItem->Bounds = BoundingBox({ 0.0f, 0.0f, 0.0f }, { 10.0f, 10.0f, 0.1f });
		renderItem->Stride = sizeof(VertexTypes::BillboardVertexType);
		//graphics->AddRenderItem(std::move(renderItem), { RenderLayer::Grass });
	}

	// Debug:
	{
		auto renderItem = std::make_unique<RenderItem>();
		renderItem->Name = "Debug";
		renderItem->Mesh = m_geometries.at("Rectangle").get();
		renderItem->Material = m_materials["NullTexture"].get();
		renderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		const auto& submesh = renderItem->Mesh->GetSubmesh("Default");
		renderItem->IndexCount = submesh.IndexCount;
		renderItem->StartIndexLocation = submesh.StartIndexLocation;
		renderItem->BaseVertexLocation = submesh.BaseVertexLocation;
		renderItem->Bounds = submesh.Bounds;
		renderItem->Stride = sizeof(VertexTypes::TextureVertexType);
		graphics->AddRenderItem(std::move(renderItem), { RenderLayer::Debug });
	}

	// Grass:
	{
		auto renderItem = std::make_unique<RenderItem>();
		renderItem->Name = "Grass01";
		renderItem->Mesh = m_geometries.at("Grass01").get();
		renderItem->Material = m_materials["Grass01"].get();
		const auto& submesh = renderItem->Mesh->GetSubmesh("Default");
		renderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		renderItem->IndexCount = submesh.IndexCount;
		renderItem->StartIndexLocation = submesh.StartIndexLocation;
		renderItem->BaseVertexLocation = submesh.BaseVertexLocation;
		renderItem->Stride = sizeof(VertexTypes::BillboardVertexType);
		graphics->AddRenderItem(std::move(renderItem), { RenderLayer::Grass });
	}

	// Simple cube:
	{
		AssimpImporter importer;
		AssimpImporter::ImportInfo importInfo;
		std::wstring filename(L"Models/Cube.fbx");
		importer.Import(graphics, d3dBase, textureManager, this, filename, importInfo);

		auto importedGeometry = m_geometries.at(Helpers::WStringToString(filename)).get();
		const auto& submesh = importedGeometry->GetSubmesh("Cube");
		const auto& materialName = importInfo.MaterialByMesh.at(AssimpImporter::BuildMeshName(filename, "Cube"));

		auto cubeRenderItem = std::make_unique<RenderItem>();
		cubeRenderItem->Name = "Cube";
		cubeRenderItem->Mesh = importedGeometry;
		cubeRenderItem->Material = m_materials.at(materialName).get();
		cubeRenderItem->Material->DiffuseMap = &textureManager["GrassDiffuseMap"];
		cubeRenderItem->Material->NormalMap = &textureManager["GrassNormalMap"];
		cubeRenderItem->Material->SpecularMap = &textureManager["GrassSpecularMap"];
		cubeRenderItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		cubeRenderItem->IndexCount = submesh.IndexCount;
		cubeRenderItem->StartIndexLocation = submesh.StartIndexLocation;
		cubeRenderItem->BaseVertexLocation = submesh.BaseVertexLocation;
		cubeRenderItem->Bounds = submesh.Bounds;

		// Instances:
		const auto size = 1;
		const auto offset = 4.0f;
		const auto start = 0.0f; // -size  * offset / 2.0f;
		cubeRenderItem->InstancesData.reserve(size * size * size);
		for (SIZE_T i = 0; i < size; ++i)
		{
			for (SIZE_T j = 0; j < size; ++j)
			{
				for (SIZE_T k = 0; k < size; ++k)
				{
					ShaderBufferTypes::InstanceData instanceData;

					XMStoreFloat4x4(&instanceData.WorldMatrix, XMMatrixTranslation(start + i * offset, start + j * offset, start + k * offset));

					cubeRenderItem->AddInstance(instanceData);
				}
			}
		}

		graphics->AddRenderItem(std::move(cubeRenderItem), { RenderLayer::NormalSpecularMapping });
	}

	// Skydome:
	{
		AssimpImporter importer;
		AssimpImporter::ImportInfo importInfo;
		std::wstring domefilename(L"Models/SkyDome.fbx");
		importer.Import(graphics, d3dBase, textureManager, this, domefilename, importInfo);

		auto domeGeometry = m_geometries.at(Helpers::WStringToString(domefilename)).get();
		const auto& domeSubmesh = domeGeometry->GetSubmesh("Sphere");

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
		XMStoreFloat4x4(&instanceData.WorldMatrix, XMMatrixScaling(2.0f, 2.0f, 2.0f));
		domeRenderItem->AddInstance(instanceData);

		graphics->AddRenderItem(std::move(domeRenderItem), { RenderLayer::SkyDome });
	}

	// AlanTree:
	{
		AssimpImporter importer;
		AssimpImporter::ImportInfo importInfo;
		std::wstring filename(L"Models/AlanTree.fbx");
		importer.Import(graphics, d3dBase, textureManager, this, filename, importInfo);

		auto importedGeometry = m_geometries.at(Helpers::WStringToString(filename)).get();

		// Trunk:
		{
			const auto& trunkSubmesh = importedGeometry->GetSubmesh("TrunkMeshData");
			const auto& trunkMaterialName = importInfo.MaterialByMesh.at(AssimpImporter::BuildMeshName(filename, "TrunkMeshData"));

			auto renderItem = std::make_unique<RenderItem>();
			renderItem->Name = "Trunk";
			renderItem->Mesh = importedGeometry;
			renderItem->Material = m_materials.at(trunkMaterialName).get();
			renderItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			renderItem->IndexCount = trunkSubmesh.IndexCount;
			renderItem->StartIndexLocation = trunkSubmesh.StartIndexLocation;
			renderItem->BaseVertexLocation = trunkSubmesh.BaseVertexLocation;
			renderItem->Bounds = trunkSubmesh.Bounds;
			graphics->AddRenderItem(std::move(renderItem), { RenderLayer::Opaque });
		}

		// Leaves:
		{
			const auto& submesh = importedGeometry->GetSubmesh("LeavesMeshData");
			const auto& materialName = importInfo.MaterialByMesh.at(AssimpImporter::BuildMeshName(filename, "LeavesMeshData"));
			auto renderItem = std::make_unique<RenderItem>();
			renderItem->Name = "Leaves";
			renderItem->Mesh = importedGeometry;
			renderItem->Material = m_materials.at(materialName).get();
			renderItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			renderItem->IndexCount = submesh.IndexCount;
			renderItem->StartIndexLocation = submesh.StartIndexLocation;
			renderItem->BaseVertexLocation = submesh.BaseVertexLocation;
			renderItem->Bounds = submesh.Bounds;
			graphics->AddRenderItem(std::move(renderItem), { RenderLayer::AlphaClipped });
		}

		// Add instances:
		const auto& instancesData = m_sceneBuilder.GetRenderItemInstances("Tree");
		AddTreeInstances(graphics, instancesData);
	}
}

void DefaultScene::InitializeLights(LightManager& lightManager)
{
	lightManager.SetAmbientLight({ 0.25f, 0.25f, 0.35f, 1.0f });
	//lightManager.AddLight(std::make_unique<Light>(Light::CreateDirectionalCastShadowsLight({ 0.6f, 0.6f, 0.6f }, { 0.57735f, -0.57735f, 0.57735f }, {0.0f, m_terrain.GetDescription().HeightMapFactor, 0.0f})));
	lightManager.AddLight(std::make_unique<Light>(Light::CreateDirectionalLight({ 0.6f, 0.6f, 0.6f }, { 0.0f, -1.0f, 1.0f }, true)));
	//lightManager.AddLight(std::make_unique<Light>(Light::CreateDirectionalLight({ 0.3f, 0.3f, 0.3f }, { 0.57735f, -0.57735f, 0.57735f })));
	//lightManager.AddLight(std::make_unique<Light>(Light::CreateDirectionalLight({ 0.15f, 0.15f, 0.15f }, { 0.0f, -0.707f, -0.707f })));
	//lightManager.AddLight(std::make_unique<Light>(Light::CreatePointLight({ 0.1f, 0.0f, 0.0f }, 2.0f, 20.0f, { 0.0f, 3.0f, 0.0f })));
	//lightManager.AddLight(std::make_unique<Light>(Light::CreateSpotLight({ 0.0f, 0.0f, 0.9f }, 2.0f, { 0.0f,-1.0f, 0.0f }, 20.0f, { 0.0f, 3.0f, 0.0f }, 8.0f)));
}

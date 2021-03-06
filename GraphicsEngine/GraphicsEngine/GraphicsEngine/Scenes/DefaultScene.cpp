﻿#include "stdafx.h"
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
#include "GraphicsEngine/NormalRenderItem.h"
#include "GraphicsEngine/CubeMappingRenderItem.h"

using namespace DirectX;
using namespace Common;
using namespace GraphicsEngine;

DefaultScene::DefaultScene(Graphics* graphics, const D3DBase& d3dBase, TextureManager& textureManager, LightManager& lightManager) :
	m_initialized(false),
	m_grassRotation(0.0f),
	m_windDirection(1.0f),
	m_sceneBuilder(L"Instances.json")
{
	InitializeTextures(d3dBase, textureManager);
	InitializeTerrain(graphics, d3dBase, textureManager);
	InitializeGeometry(d3dBase);
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

void DefaultScene::AddImmutableGeometry(std::unique_ptr<ImmutableMeshGeometry>&& geometry)
{
	m_immutableGeometries.emplace(geometry->GetName(), std::move(geometry));
}
void DefaultScene::AddBillboardGeometry(std::unique_ptr<BillboardMeshGeometry>&& geometry)
{
	m_billboardGeometries.emplace(geometry->GetName(), std::move(geometry));
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
const std::unordered_map<std::string, std::unique_ptr<ImmutableMeshGeometry>>& DefaultScene::GetImmutableGeometries() const
{
	return m_immutableGeometries;
}
const std::unordered_map<std::string, std::unique_ptr<BillboardMeshGeometry>>& DefaultScene::GetBillboardGeometries() const
{
	return m_billboardGeometries;
}
const std::unordered_map<std::string, std::unique_ptr<Material>>& DefaultScene::GetMaterials() const
{
	return m_materials;
}

void DefaultScene::AddNormalInstances(Graphics* graphics, const std::string& name, const std::initializer_list<std::string>& renderItemNames, const std::vector<SceneBuilder::RenderItemInstanceData>& instancesData, FXMMATRIX transformMatrix)
{
	std::vector<NormalRenderItem*> renderItems;
	renderItems.reserve(renderItemNames.size());
	for(const auto& renderItemName : renderItemNames)
	{
		auto renderItem = *graphics->GetNormalRenderItem(renderItemName);
		renderItem->InscreaseInstancesCapacity(instancesData.size());

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
			graphics->AddNormalRenderItemInstance(renderItem, instanceDataBuffer);

		if(m_initialized)
		{
			m_sceneBuilder.AddRenderItemInstance(name, instanceData);
		}
	}
}
void DefaultScene::AddBillboardInstances(Graphics* graphics, const std::string& name, const std::initializer_list<std::string>& renderItemNames, const std::vector<SceneBuilder::RenderItemInstanceData>& instancesData, float yOffset)
{
	std::vector<BillboardRenderItem*> renderItems;
	renderItems.reserve(renderItemNames.size());
	for (const auto& renderItemName : renderItemNames)
	{
		auto renderItem = *graphics->GetBillboardRenderItem(renderItemName);
		renderItems.push_back(renderItem);
	}

	auto instanceDataCount = instancesData.size();
	for (size_t i = 0; i < instanceDataCount; ++i)
	{
		const auto& instanceData = instancesData[i];

		BillboardMeshGeometry::VertexType instanceDataBuffer;

		const auto& position = instanceData.Position;
		instanceDataBuffer.Center = XMFLOAT3(position.x, m_terrain.GetTerrainHeight(position.x, position.y) + yOffset, position.y);
		instanceDataBuffer.Extents = XMFLOAT2(instanceData.Scale.x, instanceData.Scale.y);

		for (auto renderItem : renderItems)
			graphics->AddBillboardRenderItemInstance(renderItem, instanceDataBuffer);
		
		if (m_initialized)
		{
			m_sceneBuilder.AddRenderItemInstance(name, instanceData);
		}
	}
}
void DefaultScene::AddTreeInstances(Graphics* graphics, const std::vector<SceneBuilder::RenderItemInstanceData>& instancesData)
{
	auto scale = 0.3f;
	auto transformationMatrix = XMMatrixScaling(scale, scale, scale) * XMMatrixRotationX(XM_PI / 2.0f);

	AddNormalInstances(graphics, "Tree", { "Trunk", "Leaves", "Bark" }, instancesData, transformationMatrix);
}
void DefaultScene::RemoveLastInstance(Graphics* graphics, const std::string& itemName, const std::initializer_list<std::string>& renderItemNames)
{
	for(const auto& renderItemName : renderItemNames)
	{
		auto renderItem = graphics->GetRenderItem(renderItemName);
		(*renderItem)->RemoveLastInstance();
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

	terrainDescription.TiledTexturesNames =
	{
		{
			{ "RockDiffuseMap" },
			{ "RockNormalMap" },
			{ "RockSpecularMap" },
		},
		{
			{ "GrassDiffuseMap" },
			{ "GrassNormalMap" },
			{ "GrassSpecularMap" },
		},
		{
			{ "PathDiffuseMap" },
			{ "PathNormalMap" },
			{ "PathSpecularMap" },
			{ "PathAlphaMap" },
		},
		{
			{ "SnowNormalMap" },
		},
	};

	terrainDescription.HeightMapFilename = L"Textures/TerrainHeightMap.r16";
	terrainDescription.NormalMapFilename = L"Textures/TerrainNormalMap.png";
	terrainDescription.TangentMapFilename = L"Textures/TerrainTangentMap.png";
	terrainDescription.HeightMapWidth = 1024;
	terrainDescription.HeightMapHeight = 1024;
	terrainDescription.HeightMapFactor = 256.0f;
	terrainDescription.TiledTexelScale = 64.0f;
	m_terrain = Terrain(d3dBase, *graphics, textureManager, *this, terrainDescription);
}
void DefaultScene::InitializeGeometry(const D3DBase& d3dBase)
{
	auto device = d3dBase.GetDevice();

	// Grass:
	{
		std::array<std::string, 4> grassNames = {
			"BillboardGrass0001",
			"BillboardGrass0002",
			"BillboardBlueFlowers",
			"BillboardRedFlowers"
		};

		for (const auto& grassName : grassNames)
		{
			auto geometry = std::make_unique<BillboardMeshGeometry>();
			geometry->SetName(grassName);
			AddBillboardGeometry(std::move(geometry));
		}
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
		geometry->CreateVertexBuffer(device, vertices, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		geometry->CreateIndexBuffer(device, rectangleMeshData.Indices, DXGI_FORMAT_R32_UINT);

		SubmeshGeometry submesh;
		submesh.StartIndexLocation = 0;
		submesh.IndexCount = static_cast<uint32_t>(rectangleMeshData.Indices.size());
		submesh.BaseVertexLocation = 0;
		submesh.Bounds = MeshGeometry::CreateBoundingBoxFromMesh(vertices);
		geometry->AddSubmesh("Default", std::move(submesh));

		AddImmutableGeometry(std::move(geometry));
	}
}
void DefaultScene::InitializeTextures(const D3DBase& d3dBase, TextureManager& textureManager)
{
	auto device = d3dBase.GetDevice();

	textureManager.Create(device, "Default", L"Textures/White.dds", true);

	textureManager.Create(device, "BillboardGrass0001", L"Textures/BillboardGrass0001.dds", true);
	textureManager.Create(device, "BillboardGrass0002", L"Textures/BillboardGrass0002.dds", true);
	textureManager.Create(device, "BillboardBlueFlowers", L"Textures/BillboardBlueFlowers.dds", true);
	textureManager.Create(device, "BillboardRedFlowers", L"Textures/BillboardRedFlowers.dds", true);

	textureManager.Create(device, "SampleLeavesDiffuseMap", L"Models/SampleLeaves_1.dds", true);
	textureManager.Create(device, "SampleLeavesNormalMap", L"Models/SampleLeaves_1_Normal.dds", false);
	textureManager.Create(device, "SampleLeavesSpecularMap", L"Models/SampleLeaves_1_Spec.dds", false);

	textureManager.Create(device, "ConiferBarkDiffuseMap", L"Models/ConiferBark.dds", true);
	textureManager.Create(device, "ConiferBarkNormalMap", L"Models/ConiferBark_Normal.dds", false);

	textureManager.Create(device, "PalmBarkDiffuseMap", L"Models/PalmBark.dds", true);
	textureManager.Create(device, "PalmBarkNormalMap", L"Models/PalmBark_Normal.dds", false);

	textureManager.Create(device, "CloudsMap", L"Textures/cloud001.dds", false);
	textureManager.Create(device, "CloudsNoiseMap", L"Textures/noise001.dds", false);


	textureManager.Create(device, "RockDiffuseMap", L"Textures/rock01d.dds", true);
	textureManager.Create(device, "RockNormalMap", L"Textures/rock01n.dds", false);
	textureManager.Create(device, "RockSpecularMap", L"Textures/rock01s.dds", false);

	textureManager.Create(device, "GrassDiffuseMap", L"Textures/ground14d.dds", true);
	textureManager.Create(device, "GrassNormalMap", L"Textures/ground14n.dds", false);
	textureManager.Create(device, "GrassSpecularMap", L"Textures/ground14s.dds", false);

	textureManager.Create(device, "PathDiffuseMap", L"Textures/ground06d.dds", true);
	textureManager.Create(device, "PathNormalMap", L"Textures/ground06n.dds", false);
	textureManager.Create(device, "PathSpecularMap", L"Textures/ground06s.dds", false);
	textureManager.Create(device, "PathAlphaMap", L"Textures/ground06a.dds", false);

	textureManager.Create(device, "SnowNormalMap", L"Textures/snow01n.dds", false);
}
void DefaultScene::InitializeMaterials(TextureManager& textureManager)
{
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
		material->Name = "BillboardGrass0001";
		material->DiffuseMap = &textureManager["BillboardGrass0001"];
		material->DiffuseAlbedo = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		material->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
		material->Roughness = 0.4f;
		material->MaterialTransform = MathHelper::Identity4x4();
		AddMaterial(std::move(material));
	}
	{
		auto material = std::make_unique<Material>();
		material->Name = "BillboardGrass0002";
		material->DiffuseMap = &textureManager["BillboardGrass0002"];
		material->DiffuseAlbedo = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		material->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
		material->Roughness = 0.4f;
		material->MaterialTransform = MathHelper::Identity4x4();
		AddMaterial(std::move(material));
	}
	{
		auto material = std::make_unique<Material>();
		material->Name = "BillboardBlueFlowers";
		material->DiffuseMap = &textureManager["BillboardBlueFlowers"];
		material->DiffuseAlbedo = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		material->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
		material->Roughness = 0.4f;
		material->MaterialTransform = MathHelper::Identity4x4();
		AddMaterial(std::move(material));
	}
	{
		auto material = std::make_unique<Material>();
		material->Name = "BillboardRedFlowers";
		material->DiffuseMap = &textureManager["BillboardRedFlowers"];
		material->DiffuseAlbedo = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		material->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
		material->Roughness = 0.4f;
		material->MaterialTransform = MathHelper::Identity4x4();
		AddMaterial(std::move(material));
	}

	{
		auto material = std::make_unique<Material>();
		material->Name = "SkyDome";
		AddMaterial(std::move(material));
	}

	{
		auto material = std::make_unique<Material>();
		material->Name = "SkyClouds";
		material->DiffuseMap = &textureManager["CloudsMap"];
		material->NormalMap = &textureManager["CloudsNoiseMap"];
		AddMaterial(std::move(material));
	}

	{
		auto material = std::make_unique<Material>();
		material->Name = "Mirror";
		material->DiffuseMap = &textureManager["Default"];
		material->DiffuseAlbedo = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		material->FresnelR0 = XMFLOAT3(0.98f, 0.97f, 0.95f);
		material->Roughness = 0.1f;
		material->MaterialTransform = MathHelper::Identity4x4();
		AddMaterial(std::move(material));
	}
}

void DefaultScene::InitializeRenderItems(Graphics* graphics, const D3DBase& d3dBase, TextureManager& textureManager)
{
	auto device = d3dBase.GetDevice();

	// Debug:
	{
		auto renderItem = std::make_unique<NormalRenderItem>();
		renderItem->SetName("Debug");
		renderItem->SetMesh(m_immutableGeometries.at("Rectangle").get(), "Default");
		renderItem->SetMaterial(m_materials["NullTexture"].get());
		graphics->AddNormalRenderItem(std::move(renderItem), { RenderLayer::Debug });
	}

	// Grass:
	{
		std::array<std::string, 4> grassNames = {
			"BillboardGrass0001",
			"BillboardGrass0002",
			"BillboardBlueFlowers",
			"BillboardRedFlowers"
		};

		for(const auto& grassName : grassNames)
		{
			auto renderItem = std::make_unique<BillboardRenderItem>();
			renderItem->SetName(grassName);
			renderItem->SetMesh(m_billboardGeometries.at(grassName).get());
			renderItem->SetMaterial(m_materials[grassName].get());
			graphics->AddBillboardRenderItem(std::move(renderItem), { RenderLayer::Grass });

			// Add instances:
			const auto& instancesData = m_sceneBuilder.GetRenderItemInstances(grassName);
			AddBillboardInstances(graphics, grassName, { grassName }, instancesData, 0.5f);
		}
	}

	// Simple cube:
	/*{
		AssimpImporter importer;
		AssimpImporter::ImportInfo importInfo;
		std::wstring filename(L"Models/Cube.fbx");
		importer.Import(graphics, d3dBase, textureManager, this, filename, importInfo);

		auto importedGeometry = m_immutableGeometries.at(Helpers::WStringToString(filename)).get();

		auto renderItem = std::make_unique<NormalRenderItem>();
		renderItem->SetName("Cube");
		renderItem->SetMesh(importedGeometry, "Cube");
		renderItem->SetMaterial(m_materials["Crate01"].get());

		// Instances:
		const auto size = 1;
		const auto offset = 4.0f;
		const auto start = 0.0f; // -size  * offset / 2.0f;
		renderItem->InscreaseInstancesCapacity(size * size * size);
		for (SIZE_T i = 0; i < size; ++i)
		{
			for (SIZE_T j = 0; j < size; ++j)
			{
				for (SIZE_T k = 0; k < size; ++k)
				{
					ShaderBufferTypes::InstanceData instanceData;

					XMStoreFloat4x4(&instanceData.WorldMatrix, XMMatrixTranslation(start + i * offset, start + j * offset, start + k * offset));

					renderItem->AddInstance(instanceData);
				}
			}
		}

		graphics->AddNormalRenderItem(std::move(renderItem), { RenderLayer::NormalSpecularMapping });
	}*/

	// Skydome:
	{
		AssimpImporter importer;
		AssimpImporter::ImportInfo importInfo;
		std::wstring filename(L"Models/SkyDome.fbx");
		importer.Import(graphics, d3dBase, textureManager, this, filename, importInfo);

		auto geometry = m_immutableGeometries.at(Helpers::WStringToString(filename)).get();

		auto renderItem = std::make_unique<NormalRenderItem>();
		renderItem->SetName("SkyDome");
		renderItem->SetMesh(geometry, "Sphere");
		renderItem->SetMaterial(m_materials.at("SkyDome").get());

		graphics->AddNormalRenderItem(std::move(renderItem), { RenderLayer::SkyDome });
	}

	// Sky Clouds:
	{
		AssimpImporter importer;
		AssimpImporter::ImportInfo importInfo;
		std::wstring filename(L"Models/SkyClouds.fbx");
		importer.Import(graphics, d3dBase, textureManager, this, filename, importInfo);

		auto geometry = m_immutableGeometries.at(Helpers::WStringToString(filename)).get();

		auto renderItem = std::make_unique<NormalRenderItem>();
		renderItem->SetName("SkyClouds");
		renderItem->SetMesh(geometry, "Plane.001");
		renderItem->SetMaterial(m_materials.at("SkyClouds").get());

		graphics->AddNormalRenderItem(std::move(renderItem), { RenderLayer::SkyClouds });
	}

	// Cube map sphere:
	{
		AssimpImporter importer;
		AssimpImporter::ImportInfo importInfo;
		std::wstring filename(L"Models/Sphere.fbx");
		importer.Import(graphics, d3dBase, textureManager, this, filename, importInfo);
		auto geometry = m_immutableGeometries.at(Helpers::WStringToString(filename)).get();

		XMVECTOR position = XMVectorSet(-217.0f, 73.0f, 221.0f, 1.0f);
		auto renderItem = std::make_unique<CubeMappingRenderItem>(device, geometry, "Sphere");
		renderItem->SetName("ReflectionSphere");
		renderItem->SetMaterial(m_materials.at("Mirror").get());
		renderItem->SetPosition(position);
		
		graphics->AddCubeMappingRenderItem(std::move(renderItem), { RenderLayer::OpaqueDynamicReflectors });
	}

	// AlanTree:
	{
		/*AssimpImporter importer;
		AssimpImporter::ImportInfo importInfo;
		std::wstring filename(L"Models/AlanTree.fbx");
		importer.Import(graphics, d3dBase, textureManager, this, filename, importInfo);

		auto importedGeometry = m_immutableGeometries.at(Helpers::WStringToString(filename)).get();

		// Trunk:
		{
			const auto& trunkMaterialName = importInfo.MaterialByMesh.at(AssimpImporter::BuildMeshName(filename, "TrunkMeshData"));

			auto renderItem = std::make_unique<NormalRenderItem>();
			renderItem->SetName("Trunk");
			renderItem->SetMesh(importedGeometry, "TrunkMeshData");
			renderItem->SetMaterial(m_materials.at(trunkMaterialName).get());
			graphics->AddNormalRenderItem(std::move(renderItem), { RenderLayer::Opaque });
		}

		// Leaves:
		{
			const auto& submesh = importedGeometry->GetSubmesh("LeavesMeshData");
			const auto& materialName = importInfo.MaterialByMesh.at(AssimpImporter::BuildMeshName(filename, "LeavesMeshData"));
			auto renderItem = std::make_unique<NormalRenderItem>();
			renderItem->SetName("Leaves");
			renderItem->SetMesh(importedGeometry, "LeavesMeshData");
			renderItem->SetMaterial(m_materials.at(materialName).get());
			graphics->AddNormalRenderItem(std::move(renderItem), { RenderLayer::AlphaClipped });
		}

		// Add instances:
		const auto& instancesData = m_sceneBuilder.GetRenderItemInstances("Tree");
		AddTreeInstances(graphics, instancesData);*/
	}

	// Tree
	{
		AssimpImporter importer;
		AssimpImporter::ImportInfo importInfo;
		std::wstring filename(L"Models/Tree.fbx");
		importer.Import(graphics, d3dBase, textureManager, this, filename, importInfo);

		auto importedGeometry = m_immutableGeometries.at(Helpers::WStringToString(filename)).get();

		// Trunk:
		{
			const auto& materialName = importInfo.MaterialByMesh.at(AssimpImporter::BuildMeshName(filename, "Trunk"));
			auto pMaterial = m_materials.at(materialName).get();
			pMaterial->DiffuseMap = &textureManager["PalmBarkDiffuseMap"];
			pMaterial->NormalMap = &textureManager["PalmBarkNormalMap"];

			auto renderItem = std::make_unique<NormalRenderItem>();
			renderItem->SetName("Trunk");
			renderItem->SetMesh(importedGeometry, "Trunk");
			renderItem->SetMaterial(pMaterial);
			graphics->AddNormalRenderItem(std::move(renderItem), { RenderLayer::NormalMapping });
		}

		// Leaves:
		{
			const auto& materialName = importInfo.MaterialByMesh.at(AssimpImporter::BuildMeshName(filename, "Leaves"));
			auto pMaterial = m_materials.at(materialName).get();
			pMaterial->DiffuseMap = &textureManager["SampleLeavesDiffuseMap"];
			pMaterial->NormalMap = &textureManager["SampleLeavesNormalMap"];
			pMaterial->SpecularMap = &textureManager["SampleLeavesSpecularMap"];

			auto renderItem = std::make_unique<NormalRenderItem>();
			renderItem->SetName("Leaves");
			renderItem->SetMesh(importedGeometry, "Leaves");
			renderItem->SetMaterial(pMaterial);
			graphics->AddNormalRenderItem(std::move(renderItem), { RenderLayer::NormalSpecularMappingTransparent });
		}

		// Bark:
		{
			const auto& materialName = importInfo.MaterialByMesh.at(AssimpImporter::BuildMeshName(filename, "Bark"));
			auto pMaterial = m_materials.at(materialName).get();
			pMaterial->DiffuseMap = &textureManager["ConiferBarkDiffuseMap"];
			pMaterial->NormalMap = &textureManager["ConiferBarkNormalMap"];

			auto renderItem = std::make_unique<NormalRenderItem>();
			renderItem->SetName("Bark");
			renderItem->SetMesh(importedGeometry, "Bark");
			renderItem->SetMaterial(pMaterial);
			graphics->AddNormalRenderItem(std::move(renderItem), { RenderLayer::NormalMapping });
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
	//lightManager.AddLight(std::make_unique<Light>(Light::CreatePointLight({ 0.8f, 0.8f, 0.8f }, 50.0f, 100.0f, { -295.0f, 28.0f, 448.0f })));
	//lightManager.AddLight(std::make_unique<Light>(Light::CreatePointLight({ 0.8f, 0.8f, 0.8f }, 100.0f, 500.0f, { 220.0f - 512.0f, 27.0f, -(0.0f - 512.0f) })));
}

#include "stdafx.h"
#include "DefaultScene.h"
#include "Content/Graphics.h"
#include "Content/GeometryGenerator.h"
#include "Content/Texture.h"
#include "Content/VertexTypes.h"
#include <DirectXColors.h>

using namespace DirectX;
using namespace GraphicsEngine;

void DefaultScene::AddTextures(TextureManager* pTextureManager) const
{
	pTextureManager->AddTexture(std::make_unique<Texture>("BricksTexture", L"Textures/bricks.dds"));
	pTextureManager->AddTexture(std::make_unique<Texture>("StoneTexture", L"Textures/stone.dds"));
	pTextureManager->AddTexture(std::make_unique<Texture>("TileTexture", L"Textures/tile.dds"));
	pTextureManager->AddTexture(std::make_unique<Texture>("CrateTexture", L"Textures/wood_crate01.dds"));
}

void DefaultScene::Initialize(Graphics* graphics, const D3DBase& d3dBase, const TextureManager& textureManager)
{
	InitializeGeometry(d3dBase);
	InitializeMaterials(textureManager);
	InitializeRenderItems(graphics);
}

const std::unordered_map<std::string, std::unique_ptr<Material>>& DefaultScene::GetMaterials() const
{
	return m_materials;
}

void DefaultScene::InitializeGeometry(const D3DBase& d3dBase)
{
	// ShapeGeo:
	{
		auto box = GeometryGenerator::CreateBox(1.5f, 0.5f, 1.5f, 3);
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

		SubmeshGeometry gridSubmesh;
		gridSubmesh.IndexCount = static_cast<UINT>(grid.Indices32.size());
		gridSubmesh.StartIndexLocation = gridIndexOffset;
		gridSubmesh.BaseVertexLocation = gridVertexOffset;

		SubmeshGeometry sphereSubmesh;
		sphereSubmesh.IndexCount = static_cast<UINT>(sphere.Indices32.size());
		sphereSubmesh.StartIndexLocation = sphereIndexOffset;
		sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

		SubmeshGeometry cylinderSubmesh;
		cylinderSubmesh.IndexCount = static_cast<UINT>(cylinder.Indices32.size());
		cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
		cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;

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
		for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
		{
			vertices[k].Position = box.Vertices[i].Position;
			vertices[k].Normal = box.Vertices[i].Normal;
			vertices[k].TextureCoordinates = box.Vertices[i].TextureCoordinates;
		}

		for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
		{
			vertices[k].Position = grid.Vertices[i].Position;
			vertices[k].Normal = grid.Vertices[i].Normal;
			vertices[k].TextureCoordinates = grid.Vertices[i].TextureCoordinates;
		}

		for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
		{
			vertices[k].Position = sphere.Vertices[i].Position;
			vertices[k].Normal = sphere.Vertices[i].Normal;
			vertices[k].TextureCoordinates = sphere.Vertices[i].TextureCoordinates;
		}

		for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
		{
			vertices[k].Position = cylinder.Vertices[i].Position;
			vertices[k].Normal = cylinder.Vertices[i].Normal;
			vertices[k].TextureCoordinates = cylinder.Vertices[i].TextureCoordinates;
		}

		std::vector<std::uint16_t> indices;
		indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
		indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
		indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
		indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

		auto geo = std::make_unique<MeshGeometry>();
		geo->Name = "ShapeGeo";
		geo->Vertices = VertexBuffer(d3dBase, vertices.data(), static_cast<uint32_t>(vertices.size()), sizeof(VertexTypes::DefaultVertexType));
		geo->Indices = IndexBuffer(d3dBase, indices.data(), static_cast<uint32_t>(indices.size()), sizeof(std::uint16_t), DXGI_FORMAT::DXGI_FORMAT_R16_UINT);
		geo->DrawArgs["Box"] = boxSubmesh;
		geo->DrawArgs["Grid"] = gridSubmesh;
		geo->DrawArgs["Sphere"] = sphereSubmesh;
		geo->DrawArgs["Cylinder"] = cylinderSubmesh;

		m_geometries[geo->Name] = std::move(geo);
	}

	// Skull:
	{
		auto skull = MeshGeometry::LoadFromFile(d3dBase, "Skull", L"Models/Skull.txt");
		m_geometries[skull->Name] = std::move(skull);
	}
}
void DefaultScene::InitializeMaterials(const TextureManager& textureManager)
{
	// Bricks0:
	{
		auto bricks0 = std::make_unique<Material>();
		bricks0->Name = "Bricks0";
		bricks0->MaterialCBIndex = 0;
		bricks0->DiffuseSrvHeapIndex = textureManager.GetTexture("BricksTexture")->HeapIndex;
		bricks0->DiffuseAlbedo = XMFLOAT4(Colors::ForestGreen);
		bricks0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
		bricks0->Roughness = 0.1f;
		m_materials[bricks0->Name] = std::move(bricks0);
	}

	// Stone0:
	{
		auto stone0 = std::make_unique<Material>();
		stone0->Name = "Stone0";
		stone0->MaterialCBIndex = 1;
		stone0->DiffuseSrvHeapIndex = textureManager.GetTexture("StoneTexture")->HeapIndex;
		stone0->DiffuseAlbedo = XMFLOAT4(Colors::LightSteelBlue);
		stone0->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
		stone0->Roughness = 0.3f;
		m_materials[stone0->Name] = std::move(stone0);
	}

	// Tile0:
	{
		auto tile0 = std::make_unique<Material>();
		tile0->Name = "Tile0";
		tile0->MaterialCBIndex = 2;
		tile0->DiffuseSrvHeapIndex = textureManager.GetTexture("TileTexture")->HeapIndex;
		tile0->DiffuseAlbedo = XMFLOAT4(Colors::LightGray);
		tile0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
		tile0->Roughness = 0.2f;
		m_materials[tile0->Name] = std::move(tile0);
	}

	// Crate0:
	{
		auto crate0 = std::make_unique<Material>();
		crate0->Name = "Crate0";
		crate0->MaterialCBIndex = 3;
		crate0->DiffuseSrvHeapIndex = textureManager.GetTexture("CrateTexture")->HeapIndex;
		crate0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		crate0->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
		crate0->Roughness = 0.2f;
		m_materials[crate0->Name] = std::move(crate0);
	}

	// SkullMaterial
	{
		auto skullMaterial = std::make_unique<Material>();
		skullMaterial->Name = "SkullMaterial";
		skullMaterial->MaterialCBIndex = 4;
		skullMaterial->DiffuseSrvHeapIndex = textureManager.GetTexture("CrateTexture")->HeapIndex;
		skullMaterial->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		skullMaterial->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
		skullMaterial->Roughness = 0.3f;
		m_materials[skullMaterial->Name] = std::move(skullMaterial);
	}
}
void DefaultScene::InitializeRenderItems(Graphics* graphics)
{
	// Box:
	{
		auto boxRenderItem = std::make_unique<RenderItem>();
		XMStoreFloat4x4(&boxRenderItem->WorldMatrix, XMMatrixScaling(2.0f, 2.0f, 2.0f)*XMMatrixTranslation(0.0f, 0.5f, 0.0f));
		XMStoreFloat4x4(&boxRenderItem->TextureTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
		boxRenderItem->ObjectCBIndex = 0;
		boxRenderItem->Mesh = m_geometries["ShapeGeo"].get();
		boxRenderItem->Material = m_materials["Crate0"].get();
		boxRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		boxRenderItem->IndexCount = boxRenderItem->Mesh->DrawArgs["Box"].IndexCount;
		boxRenderItem->StartIndexLocation = boxRenderItem->Mesh->DrawArgs["Box"].StartIndexLocation;
		boxRenderItem->BaseVertexLocation = boxRenderItem->Mesh->DrawArgs["Box"].BaseVertexLocation;
		graphics->AddRenderItem(std::move(boxRenderItem), { RenderLayer::Opaque });
	}

	// Grid:
	{
		auto gridRenderItem = std::make_unique<RenderItem>();
		gridRenderItem->WorldMatrix = MathHelper::Identity4x4();
		XMStoreFloat4x4(&gridRenderItem->TextureTransform, XMMatrixScaling(8.0f, 8.0f, 1.0f));
		gridRenderItem->ObjectCBIndex = 1;
		gridRenderItem->Mesh = m_geometries["ShapeGeo"].get();
		gridRenderItem->Material = m_materials["Tile0"].get();
		gridRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		gridRenderItem->IndexCount = gridRenderItem->Mesh->DrawArgs["Grid"].IndexCount;
		gridRenderItem->StartIndexLocation = gridRenderItem->Mesh->DrawArgs["Grid"].StartIndexLocation;
		gridRenderItem->BaseVertexLocation = gridRenderItem->Mesh->DrawArgs["Grid"].BaseVertexLocation;
		graphics->AddRenderItem(std::move(gridRenderItem), { RenderLayer::Opaque });
	}

	// Skull:
	{
		auto skullRenderItem = std::make_unique<RenderItem>();
		XMStoreFloat4x4(&skullRenderItem->WorldMatrix, XMMatrixScaling(0.5f, 0.5f, 0.5f)*XMMatrixTranslation(0.0f, 1.0f, 0.0f));
		skullRenderItem->TextureTransform = MathHelper::Identity4x4();
		skullRenderItem->ObjectCBIndex = 2;
		skullRenderItem->Material = m_materials["SkullMaterial"].get();
		skullRenderItem->Mesh = m_geometries["Skull"].get();
		skullRenderItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		skullRenderItem->IndexCount = skullRenderItem->Mesh->DrawArgs["Skull"].IndexCount;
		skullRenderItem->StartIndexLocation = skullRenderItem->Mesh->DrawArgs["Skull"].StartIndexLocation;
		skullRenderItem->BaseVertexLocation = skullRenderItem->Mesh->DrawArgs["Skull"].BaseVertexLocation;
		graphics->AddRenderItem(std::move(skullRenderItem), { RenderLayer::Opaque });
	}

	UINT objCBIndex = 3;
	for (auto i = 0; i < 5; ++i)
	{
		// Left Cylinder:
		{
			auto leftCylinderWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i*5.0f);
			auto leftCylinderRenderItem = std::make_unique<RenderItem>();
			XMStoreFloat4x4(&leftCylinderRenderItem->WorldMatrix, leftCylinderWorld);
			leftCylinderRenderItem->TextureTransform = MathHelper::Identity4x4();
			leftCylinderRenderItem->ObjectCBIndex = objCBIndex++;
			leftCylinderRenderItem->Mesh = m_geometries["ShapeGeo"].get();
			leftCylinderRenderItem->Material = m_materials["Bricks0"].get();
			leftCylinderRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			leftCylinderRenderItem->IndexCount = leftCylinderRenderItem->Mesh->DrawArgs["Cylinder"].IndexCount;
			leftCylinderRenderItem->StartIndexLocation = leftCylinderRenderItem->Mesh->DrawArgs["Cylinder"].StartIndexLocation;
			leftCylinderRenderItem->BaseVertexLocation = leftCylinderRenderItem->Mesh->DrawArgs["Cylinder"].BaseVertexLocation;
			graphics->AddRenderItem(std::move(leftCylinderRenderItem), { RenderLayer::Opaque });
		}

		// Right Cylinder:
		{
			auto rightCylinderWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i*5.0f);
			auto rightCylinderRenderItem = std::make_unique<RenderItem>();
			XMStoreFloat4x4(&rightCylinderRenderItem->WorldMatrix, rightCylinderWorld);
			rightCylinderRenderItem->TextureTransform = MathHelper::Identity4x4();
			rightCylinderRenderItem->ObjectCBIndex = objCBIndex++;
			rightCylinderRenderItem->Mesh = m_geometries["ShapeGeo"].get();
			rightCylinderRenderItem->Material = m_materials["Bricks0"].get();
			rightCylinderRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			rightCylinderRenderItem->IndexCount = rightCylinderRenderItem->Mesh->DrawArgs["Cylinder"].IndexCount;
			rightCylinderRenderItem->StartIndexLocation = rightCylinderRenderItem->Mesh->DrawArgs["Cylinder"].StartIndexLocation;
			rightCylinderRenderItem->BaseVertexLocation = rightCylinderRenderItem->Mesh->DrawArgs["Cylinder"].BaseVertexLocation;
			graphics->AddRenderItem(std::move(rightCylinderRenderItem), { RenderLayer::Opaque });
		}

		// Left Sphere:
		{
			auto leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i*5.0f);
			auto leftSphereRenderItem = std::make_unique<RenderItem>();
			XMStoreFloat4x4(&leftSphereRenderItem->WorldMatrix, leftSphereWorld);
			leftSphereRenderItem->TextureTransform = MathHelper::Identity4x4();
			leftSphereRenderItem->ObjectCBIndex = objCBIndex++;
			leftSphereRenderItem->Mesh = m_geometries["ShapeGeo"].get();
			leftSphereRenderItem->Material = m_materials["Stone0"].get();
			leftSphereRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			leftSphereRenderItem->IndexCount = leftSphereRenderItem->Mesh->DrawArgs["Sphere"].IndexCount;
			leftSphereRenderItem->StartIndexLocation = leftSphereRenderItem->Mesh->DrawArgs["Sphere"].StartIndexLocation;
			leftSphereRenderItem->BaseVertexLocation = leftSphereRenderItem->Mesh->DrawArgs["Sphere"].BaseVertexLocation;
			graphics->AddRenderItem(std::move(leftSphereRenderItem), { RenderLayer::Opaque });
		}

		// Right Sphere:
		{
			auto rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i*5.0f);
			auto rightSphereRenderItem = std::make_unique<RenderItem>();
			XMStoreFloat4x4(&rightSphereRenderItem->WorldMatrix, rightSphereWorld);
			rightSphereRenderItem->TextureTransform = MathHelper::Identity4x4();
			rightSphereRenderItem->ObjectCBIndex = objCBIndex++;
			rightSphereRenderItem->Mesh = m_geometries["ShapeGeo"].get();
			rightSphereRenderItem->Material = m_materials["Stone0"].get();
			rightSphereRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			rightSphereRenderItem->IndexCount = rightSphereRenderItem->Mesh->DrawArgs["Sphere"].IndexCount;
			rightSphereRenderItem->StartIndexLocation = rightSphereRenderItem->Mesh->DrawArgs["Sphere"].StartIndexLocation;
			rightSphereRenderItem->BaseVertexLocation = rightSphereRenderItem->Mesh->DrawArgs["Sphere"].BaseVertexLocation;
			graphics->AddRenderItem(std::move(rightSphereRenderItem), { RenderLayer::Opaque });
		}
	}
}

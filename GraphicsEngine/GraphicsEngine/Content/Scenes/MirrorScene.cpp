#include "stdafx.h"
#include "MirrorScene.h"
#include "Content/Texture.h"
#include "Content/Graphics.h"
#include "Content/VertexTypes.h"
#include "Content/TextureManager.h"

#include <array>

using namespace DirectX;
using namespace GraphicsEngine;

void MirrorScene::AddTextures(TextureManager* pTextureManager) const
{
	pTextureManager->AddTexture(std::make_unique<Texture>("BricksTexture", L"Textures/bricks3.dds"));
	pTextureManager->AddTexture(std::make_unique<Texture>("CheckboardTexture", L"Textures/checkboard.dds"));
	pTextureManager->AddTexture(std::make_unique<Texture>("IceTexture", L"Textures/ice.dds"));
	pTextureManager->AddTexture(std::make_unique<Texture>("White1x1Texture", L"Textures/white1x1.dds"));
}

void MirrorScene::Initialize(Graphics* graphics, const D3DBase& d3dBase, const TextureManager& textureManager)
{
	InitializeGeometry(d3dBase);
	InitializeMaterials(textureManager);
	InitializeRenderItems(graphics);
}

const std::unordered_map<std::string, std::unique_ptr<Material>>& MirrorScene::GetMaterials() const
{
	return m_materials;
}

void MirrorScene::InitializeGeometry(const D3DBase& d3dBase)
{
	// Wall, mirror, wall and floor:
	{
		// Create and specify geometry.  For this sample we draw a floor
		// and a wall with a mirror on it.  We put the floor, wall, and
		// mirror geometry in one vertex buffer.
		//
		//   |--------------|
		//   |              |
		//   |----|----|----|
		//   |Wall|Mirr|Wall|
		//   |    | or |    |
		//   /--------------/
		//  /   Floor      /
		// /--------------/

		using Vertex = VertexTypes::DefaultVertexType;

		std::array<Vertex, 20> vertices =
		{
			// Floor: Observe we tile texture coordinates.
			Vertex(-3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f), // 0 
			Vertex(-3.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f),
			Vertex(7.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f),
			Vertex(7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 4.0f, 4.0f),

			// Wall: Observe we tile texture coordinates, and that we
			// leave a gap in the middle for the mirror.
			Vertex(-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f), // 4
			Vertex(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
			Vertex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f),
			Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 2.0f),

			Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f), // 8 
			Vertex(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
			Vertex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f),
			Vertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 2.0f),

			Vertex(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f), // 12
			Vertex(-3.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
			Vertex(7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f),
			Vertex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 1.0f),

			// Mirror
			Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f), // 16
			Vertex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
			Vertex(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f),
			Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f)
		};

		std::array<std::int16_t, 30> indices =
		{
			// Floor
			0, 1, 2,
			0, 2, 3,

			// Walls
			4, 5, 6,
			4, 6, 7,

			8, 9, 10,
			8, 10, 11,

			12, 13, 14,
			12, 14, 15,

			// Mirror
			16, 17, 18,
			16, 18, 19
		};

		SubmeshGeometry floorSubmesh;
		floorSubmesh.IndexCount = 6;
		floorSubmesh.StartIndexLocation = 0;
		floorSubmesh.BaseVertexLocation = 0;

		SubmeshGeometry wallSubmesh;
		wallSubmesh.IndexCount = 18;
		wallSubmesh.StartIndexLocation = 6;
		wallSubmesh.BaseVertexLocation = 0;

		SubmeshGeometry mirrorSubmesh;
		mirrorSubmesh.IndexCount = 6;
		mirrorSubmesh.StartIndexLocation = 24;
		mirrorSubmesh.BaseVertexLocation = 0;

		auto geo = std::make_unique<MeshGeometry>();
		geo->Name = "Room Geometry";
		geo->Vertices = VertexBuffer(d3dBase, vertices.data(), vertices.size(), sizeof(Vertex));
		geo->Indices = IndexBuffer(d3dBase, indices.data(), indices.size(), sizeof(std::uint16_t), DXGI_FORMAT::DXGI_FORMAT_R16_UINT);
		geo->DrawArgs["Floor"] = floorSubmesh;
		geo->DrawArgs["Wall"] = wallSubmesh;
		geo->DrawArgs["Mirror"] = mirrorSubmesh;

		m_geometries[geo->Name] = std::move(geo);		
	}

	// Skull:
	{
		auto skull = MeshGeometry::LoadFromFile(d3dBase, "Skull", L"Models/Skull.txt");
		m_geometries[skull->Name] = std::move(skull);
	}
}
void MirrorScene::InitializeMaterials(const TextureManager& textureManager)
{
	auto bricks = std::make_unique<Material>();
	bricks->Name = "Bricks";
	bricks->MaterialIndex = 0;
	bricks->DiffuseSrvHeapIndex = textureManager.GetTexture("BricksTexture")->HeapIndex;
	bricks->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	bricks->Roughness = 0.25f;
	m_materials[bricks->Name] = std::move(bricks);

	auto checkertile = std::make_unique<Material>();
	checkertile->Name = "Checker Tile";
	checkertile->MaterialIndex = 1;
	checkertile->DiffuseSrvHeapIndex = textureManager.GetTexture("CheckboardTexture")->HeapIndex;
	checkertile->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	checkertile->FresnelR0 = XMFLOAT3(0.07f, 0.07f, 0.07f);
	checkertile->Roughness = 0.3f;
	m_materials[checkertile->Name] = std::move(checkertile);

	auto icemirror = std::make_unique<Material>();
	icemirror->Name = "Ice Mirror";
	icemirror->MaterialIndex = 2;
	icemirror->DiffuseSrvHeapIndex = textureManager.GetTexture("CheckboardTexture")->HeapIndex;
	icemirror->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f);
	icemirror->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	icemirror->Roughness = 0.5f;
	m_materials[icemirror->Name] = std::move(icemirror);

	auto skullMat = std::make_unique<Material>();
	skullMat->Name = "Skull Material";
	skullMat->MaterialIndex = 3;
	skullMat->DiffuseSrvHeapIndex = textureManager.GetTexture("White1x1Texture")->HeapIndex;
	skullMat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	skullMat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	skullMat->Roughness = 0.3f;
	m_materials[skullMat->Name] = std::move(skullMat);

	auto shadowMat = std::make_unique<Material>();
	shadowMat->Name = "Shadow Material";
	shadowMat->MaterialIndex = 4;
	shadowMat->DiffuseSrvHeapIndex = textureManager.GetTexture("White1x1Texture")->HeapIndex;
	shadowMat->DiffuseAlbedo = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
	shadowMat->FresnelR0 = XMFLOAT3(0.001f, 0.001f, 0.001f);
	shadowMat->Roughness = 0.0f;
	m_materials[shadowMat->Name] = std::move(shadowMat);
}
void MirrorScene::InitializeRenderItems(Graphics* graphics)
{
	// Floor:
	{
		auto floorRenderItem = std::make_unique<RenderItem>();
		floorRenderItem->Mesh = m_geometries["Room Geometry"].get();
		floorRenderItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		const auto& submesh = floorRenderItem->Mesh->DrawArgs["Floor"];
		floorRenderItem->IndexCount = submesh.IndexCount;
		floorRenderItem->StartIndexLocation = submesh.StartIndexLocation;
		floorRenderItem->BaseVertexLocation = submesh.BaseVertexLocation;
		floorRenderItem->Bounds = MeshGeometry::CreateBoundingBoxFromMesh<VertexTypes::DefaultVertexType, uint16_t>(*floorRenderItem->Mesh, submesh);

		// Add instances:
		{
			BufferTypes::InstanceData floorInstanceData;
			XMStoreFloat4x4(&floorInstanceData.WorldMatrix, XMMatrixIdentity());
			XMStoreFloat4x4(&floorInstanceData.TextureTransform, XMMatrixIdentity());
			floorInstanceData.MaterialIndex = m_materials["Checker Tile"]->MaterialIndex;
			floorRenderItem->AddInstance(floorInstanceData);
		}

		graphics->AddRenderItem(std::move(floorRenderItem), { RenderLayer::Opaque });
	}

	// Walls:
	{
		auto wallsRenderItem = std::make_unique<RenderItem>();
		wallsRenderItem->Mesh = m_geometries["Room Geometry"].get();
		wallsRenderItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		const auto& submesh = wallsRenderItem->Mesh->DrawArgs["Wall"];
		wallsRenderItem->IndexCount = submesh.IndexCount;
		wallsRenderItem->StartIndexLocation = submesh.StartIndexLocation;
		wallsRenderItem->BaseVertexLocation = submesh.BaseVertexLocation;
		wallsRenderItem->Bounds = MeshGeometry::CreateBoundingBoxFromMesh<VertexTypes::DefaultVertexType, uint16_t>(*wallsRenderItem->Mesh, submesh);

		// Add instances:
		{
			BufferTypes::InstanceData wallsInstanceData;
			XMStoreFloat4x4(&wallsInstanceData.WorldMatrix, XMMatrixIdentity());
			XMStoreFloat4x4(&wallsInstanceData.TextureTransform, XMMatrixIdentity());
			wallsInstanceData.MaterialIndex = m_materials["Bricks"]->MaterialIndex;
			wallsRenderItem->AddInstance(wallsInstanceData);
		}

		graphics->AddRenderItem(std::move(wallsRenderItem), { RenderLayer::Opaque });
	}

	// Skull:
	{
		auto skullRotate = XMMatrixRotationY(0.5f*MathHelper::Pi);
		auto skullScale = XMMatrixScaling(0.45f, 0.45f, 0.45f);
		auto skullOffset = XMMatrixTranslation(0.0f, 1.0f, -5.0f);
		auto skullWorld = skullRotate * skullScale * skullOffset;

		auto skullRenderItem = std::make_unique<RenderItem>();
		skullRenderItem->Mesh = m_geometries["Skull"].get();
		skullRenderItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		const auto& submesh = skullRenderItem->Mesh->DrawArgs["Skull"];
		skullRenderItem->IndexCount = submesh.IndexCount;
		skullRenderItem->StartIndexLocation = submesh.StartIndexLocation;
		skullRenderItem->BaseVertexLocation = submesh.BaseVertexLocation;
		skullRenderItem->Bounds = MeshGeometry::CreateBoundingBoxFromMesh<VertexTypes::DefaultVertexType, uint32_t>(*skullRenderItem->Mesh, submesh);

		// Add instances:
		{
			BufferTypes::InstanceData skullInstanceData;
			XMStoreFloat4x4(&skullInstanceData.WorldMatrix, skullWorld);
			XMStoreFloat4x4(&skullInstanceData.TextureTransform, XMMatrixIdentity());
			skullInstanceData.MaterialIndex = m_materials["Skull Material"]->MaterialIndex;
			skullRenderItem->AddInstance(skullInstanceData);
		}

		graphics->AddRenderItem(std::move(skullRenderItem), { RenderLayer::Opaque });
	}

	// Mirror:
	{
		auto mirrorRenderItem = std::make_unique<RenderItem>();
		mirrorRenderItem->Mesh = m_geometries["Room Geometry"].get();
		mirrorRenderItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		const auto& submesh = mirrorRenderItem->Mesh->DrawArgs["Mirror"];
		mirrorRenderItem->IndexCount = submesh.IndexCount;
		mirrorRenderItem->StartIndexLocation = submesh.StartIndexLocation;
		mirrorRenderItem->BaseVertexLocation = submesh.BaseVertexLocation;
		mirrorRenderItem->Bounds = MeshGeometry::CreateBoundingBoxFromMesh<VertexTypes::DefaultVertexType, uint16_t>(*mirrorRenderItem->Mesh, submesh);

		// Add instances:
		{
			BufferTypes::InstanceData mirrorInstanceData;
			XMStoreFloat4x4(&mirrorInstanceData.WorldMatrix, XMMatrixIdentity());
			XMStoreFloat4x4(&mirrorInstanceData.TextureTransform, XMMatrixIdentity());
			mirrorInstanceData.MaterialIndex = m_materials["Ice Mirror"]->MaterialIndex;
			mirrorRenderItem->AddInstance(mirrorInstanceData);
		}

		graphics->AddRenderItem(std::move(mirrorRenderItem), { RenderLayer::Transparent, RenderLayer::Mirrors });
	}
}

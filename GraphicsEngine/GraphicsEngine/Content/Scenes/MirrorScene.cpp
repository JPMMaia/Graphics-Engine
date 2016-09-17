#include "stdafx.h"
#include "MirrorScene.h"
#include "Content/Texture.h"
#include "Content/Graphics.h"
#include "Content/VertexTypes.h"

#include <array>

using namespace DirectX;
using namespace GraphicsEngine;

MirrorScene::MirrorScene(Graphics* graphics, const D3DBase& d3dBase, TextureHeap* textureHeap)
{
	InitializeGeometry(d3dBase);
	InitializeTextures(d3dBase, textureHeap);
	InitializeMaterials();
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
void MirrorScene::InitializeTextures(const D3DBase& d3dBase, TextureHeap* textureHeap)
{
	m_textureHeapIndexOffset = static_cast<int>(textureHeap->GetTextureCount());
	textureHeap->AddTexture(std::make_unique<Texture>(d3dBase, "BricksTexture", L"Textures/bricks3.dds", m_textureHeapIndexOffset));
	textureHeap->AddTexture(std::make_unique<Texture>(d3dBase, "CheckboardTexture", L"Textures/checkboard.dds", m_textureHeapIndexOffset + 1));
	textureHeap->AddTexture(std::make_unique<Texture>(d3dBase, "IceTexture", L"Textures/ice.dds", m_textureHeapIndexOffset + 2));
	textureHeap->AddTexture(std::make_unique<Texture>(d3dBase, "White1x1Texture", L"Textures/white1x1.dds", m_textureHeapIndexOffset + 3));
}
void MirrorScene::InitializeMaterials()
{
	auto bricks = std::make_unique<Material>();
	bricks->Name = "Bricks";
	bricks->MaterialCBIndex = 0;
	bricks->DiffuseSrvHeapIndex = m_textureHeapIndexOffset;
	bricks->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	bricks->Roughness = 0.25f;
	m_materials[bricks->Name] = std::move(bricks);

	auto checkertile = std::make_unique<Material>();
	checkertile->Name = "Checker Tile";
	checkertile->MaterialCBIndex = 1;
	checkertile->DiffuseSrvHeapIndex = m_textureHeapIndexOffset + 1;
	checkertile->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	checkertile->FresnelR0 = XMFLOAT3(0.07f, 0.07f, 0.07f);
	checkertile->Roughness = 0.3f;
	m_materials[checkertile->Name] = std::move(checkertile);

	auto icemirror = std::make_unique<Material>();
	icemirror->Name = "Ice Mirror";
	icemirror->MaterialCBIndex = 2;
	icemirror->DiffuseSrvHeapIndex = m_textureHeapIndexOffset + 2;
	icemirror->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f);
	icemirror->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	icemirror->Roughness = 0.5f;
	m_materials[icemirror->Name] = std::move(icemirror);

	auto skullMat = std::make_unique<Material>();
	skullMat->Name = "Skull Material";
	skullMat->MaterialCBIndex = 3;
	skullMat->DiffuseSrvHeapIndex = m_textureHeapIndexOffset + 3;
	skullMat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	skullMat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	skullMat->Roughness = 0.3f;
	m_materials[skullMat->Name] = std::move(skullMat);

	auto shadowMat = std::make_unique<Material>();
	shadowMat->Name = "Shadow Material";
	shadowMat->MaterialCBIndex = 4;
	shadowMat->DiffuseSrvHeapIndex = m_textureHeapIndexOffset + 3;
	shadowMat->DiffuseAlbedo = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
	shadowMat->FresnelR0 = XMFLOAT3(0.001f, 0.001f, 0.001f);
	shadowMat->Roughness = 0.0f;
	m_materials[shadowMat->Name] = std::move(shadowMat);
}
void MirrorScene::InitializeRenderItems(Graphics* graphics)
{
	// Floor:
	{
		auto floorRitem = std::make_unique<RenderItem>();
		floorRitem->WorldMatrix = MathHelper::Identity4x4();
		floorRitem->TextureTransform = MathHelper::Identity4x4();
		floorRitem->ObjectCBIndex = 0;
		floorRitem->Material = m_materials["Checker Tile"].get();
		floorRitem->Mesh = m_geometries["Room Geometry"].get();
		floorRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		floorRitem->IndexCount = floorRitem->Mesh->DrawArgs["Floor"].IndexCount;
		floorRitem->StartIndexLocation = floorRitem->Mesh->DrawArgs["Floor"].StartIndexLocation;
		floorRitem->BaseVertexLocation = floorRitem->Mesh->DrawArgs["Floor"].BaseVertexLocation;
		graphics->AddRenderItem(std::move(floorRitem), { RenderLayer::Opaque });
	}

	// Walls:
	{
		auto wallsRitem = std::make_unique<RenderItem>();
		wallsRitem->WorldMatrix = MathHelper::Identity4x4();
		wallsRitem->TextureTransform = MathHelper::Identity4x4();
		wallsRitem->ObjectCBIndex = 1;
		wallsRitem->Material = m_materials["Bricks"].get();
		wallsRitem->Mesh = m_geometries["Room Geometry"].get();
		wallsRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		wallsRitem->IndexCount = wallsRitem->Mesh->DrawArgs["Wall"].IndexCount;
		wallsRitem->StartIndexLocation = wallsRitem->Mesh->DrawArgs["Wall"].StartIndexLocation;
		wallsRitem->BaseVertexLocation = wallsRitem->Mesh->DrawArgs["Wall"].BaseVertexLocation;
		graphics->AddRenderItem(std::move(wallsRitem), { RenderLayer::Opaque });
	}

	// Skull:
	{
		auto skullRotate = XMMatrixRotationY(0.5f*MathHelper::Pi);
		auto skullScale = XMMatrixScaling(0.45f, 0.45f, 0.45f);
		auto skullOffset = XMMatrixTranslation(0.0f, 1.0f, -5.0f);
		auto skullWorld = skullRotate*skullScale*skullOffset;

		auto skullRitem = std::make_unique<RenderItem>();
		XMStoreFloat4x4(&skullRitem->WorldMatrix, skullWorld);
		skullRitem->TextureTransform = MathHelper::Identity4x4();
		skullRitem->ObjectCBIndex = 2;
		skullRitem->Material = m_materials["Skull Material"].get();
		skullRitem->Mesh = m_geometries["Skull"].get();
		skullRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		skullRitem->IndexCount = skullRitem->Mesh->DrawArgs["Skull"].IndexCount;
		skullRitem->StartIndexLocation = skullRitem->Mesh->DrawArgs["Skull"].StartIndexLocation;
		skullRitem->BaseVertexLocation = skullRitem->Mesh->DrawArgs["Skull"].BaseVertexLocation;

		// Reflected skull:
		{
			auto mirrorPlane = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
			auto reflection = XMMatrixReflect(mirrorPlane);

			// Reflected skull will have different world matrix, so it needs to be its own render item.
			auto reflectedSkullRitem = std::make_unique<RenderItem>();
			*reflectedSkullRitem = *skullRitem;
			XMStoreFloat4x4(&reflectedSkullRitem->WorldMatrix, skullWorld * reflection);
			reflectedSkullRitem->ObjectCBIndex = 3;
			graphics->AddRenderItem(std::move(reflectedSkullRitem), { RenderLayer::Reflected });
		}

		// Shadowed skull:
		{
			auto shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // xz plane
			auto toMainLight = XMVectorSet(-0.57735f, 0.57735f, -0.57735f, 0.0f);
			auto shadowMatrix = XMMatrixShadow(shadowPlane, toMainLight);
			auto shadowOffsetY = XMMatrixTranslation(0.0f, 0.005f, 0.0f);

			// Shadowed skull will have different world matrix, so it needs to be its own render item.
			auto shadowedSkullRitem = std::make_unique<RenderItem>();
			*shadowedSkullRitem = *skullRitem;
			XMStoreFloat4x4(&shadowedSkullRitem->WorldMatrix, skullWorld * shadowMatrix * shadowOffsetY);
			shadowedSkullRitem->ObjectCBIndex = 4;
			shadowedSkullRitem->Material = m_materials["Shadow Material"].get();
			graphics->AddRenderItem(std::move(shadowedSkullRitem), { RenderLayer::Shadow });
		}

		graphics->AddRenderItem(std::move(skullRitem), { RenderLayer::Opaque });
	}

	// Mirror:
	{
		auto mirrorRitem = std::make_unique<RenderItem>();
		mirrorRitem->WorldMatrix = MathHelper::Identity4x4();
		mirrorRitem->TextureTransform = MathHelper::Identity4x4();
		mirrorRitem->ObjectCBIndex = 5;
		mirrorRitem->Material = m_materials["Ice Mirror"].get();
		mirrorRitem->Mesh = m_geometries["Room Geometry"].get();
		mirrorRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		mirrorRitem->IndexCount = mirrorRitem->Mesh->DrawArgs["Mirror"].IndexCount;
		mirrorRitem->StartIndexLocation = mirrorRitem->Mesh->DrawArgs["Mirror"].StartIndexLocation;
		mirrorRitem->BaseVertexLocation = mirrorRitem->Mesh->DrawArgs["Mirror"].BaseVertexLocation;
		graphics->AddRenderItem(std::move(mirrorRitem), { RenderLayer::Transparent, RenderLayer::Mirrors });
	}
}

#include "stdafx.h"
#include "DefaultScene.h"
#include "GraphicsEngine/Graphics.h"
#include "GraphicsEngine/GeometryGenerator.h"
#include "GraphicsEngine/VertexTypes.h"
#include "GraphicsEngine/RenderItem.h"

using namespace DirectX;
using namespace GraphicsEngine;

DefaultScene::DefaultScene(Graphics* graphics, const D3DBase& d3dBase)
{
	Initialize(graphics, d3dBase);
}

const std::unordered_map<std::string, std::unique_ptr<Material>>& DefaultScene::GetMaterials() const
{
	return m_materials;
}

void DefaultScene::Initialize(Graphics* graphics, const D3DBase& d3dBase)
{
	InitializeGeometry(d3dBase);
	InitializeMaterials();
	InitializeRenderItems(graphics);
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
		for (SIZE_T i = 0; i < box.Vertices.size(); ++i, ++k)
		{
			vertices[k].Position = box.Vertices[i].Position;
			vertices[k].Normal = box.Vertices[i].Normal;
		}

		for (SIZE_T i = 0; i < grid.Vertices.size(); ++i, ++k)
		{
			vertices[k].Position = grid.Vertices[i].Position;
			vertices[k].Normal = grid.Vertices[i].Normal;
		}

		for (SIZE_T i = 0; i < sphere.Vertices.size(); ++i, ++k)
		{
			vertices[k].Position = sphere.Vertices[i].Position;
			vertices[k].Normal = sphere.Vertices[i].Normal;
		}

		for (SIZE_T i = 0; i < cylinder.Vertices.size(); ++i, ++k)
		{
			vertices[k].Position = cylinder.Vertices[i].Position;
			vertices[k].Normal = cylinder.Vertices[i].Normal;
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
void DefaultScene::InitializeMaterials()
{
	auto bricks = std::make_unique<Material>();
	bricks->Name = "Bricks";
	bricks->MaterialIndex = 0;
	bricks->DiffuseSrvHeapIndex = -1; // TODO
	bricks->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	bricks->Roughness = 0.25f;
	m_materials[bricks->Name] = std::move(bricks);
}

void DefaultScene::InitializeRenderItems(Graphics* graphics)
{
	// Box:
	{
		auto boxRenderItem = std::make_unique<RenderItem>();
		XMStoreFloat4x4(&boxRenderItem->WorldMatrix, XMMatrixScaling(2.0f, 2.0f, 2.0f)*XMMatrixTranslation(0.0f, 0.5f, 0.0f));
		boxRenderItem->Mesh = m_geometries["ShapeGeo"].get();
		boxRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		boxRenderItem->IndexCount = boxRenderItem->Mesh->Submeshes.at("Box").IndexCount;
		boxRenderItem->StartIndexLocation = boxRenderItem->Mesh->Submeshes.at("Box").StartIndexLocation;
		boxRenderItem->BaseVertexLocation = boxRenderItem->Mesh->Submeshes.at("Box").BaseVertexLocation;
		boxRenderItem->ObjectBufferIndex = 0;
		boxRenderItem->MaterialBufferIndex = m_materials["Bricks"]->MaterialIndex;

		graphics->AddRenderItem(std::move(boxRenderItem), { RenderLayer::Opaque });
	}

	// Sphere:
	{
		auto sphereRenderItem = std::make_unique<RenderItem>();
		sphereRenderItem->Mesh = m_geometries["ShapeGeo"].get();
		sphereRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		sphereRenderItem->IndexCount = sphereRenderItem->Mesh->Submeshes.at("Sphere").IndexCount;
		sphereRenderItem->StartIndexLocation = sphereRenderItem->Mesh->Submeshes.at("Sphere").StartIndexLocation;
		sphereRenderItem->BaseVertexLocation = sphereRenderItem->Mesh->Submeshes.at("Sphere").BaseVertexLocation;
		sphereRenderItem->ObjectBufferIndex = 1;
		sphereRenderItem->MaterialBufferIndex = m_materials["Bricks"]->MaterialIndex;
		XMStoreFloat4x4(&sphereRenderItem->WorldMatrix, XMMatrixTranslation(0.0f, 5.0f, 0.0f));

		graphics->AddRenderItem(std::move(sphereRenderItem), { RenderLayer::Opaque });
	}
}

#include "stdafx.h"
#include "MeshGeometry.h"
#include "VertexTypes.h"
#include <D3Dcompiler.h>

using namespace GraphicsEngine;

void MeshGeometry::DisposeUploaders()
{
	this->Vertices.DisposeUploadBuffer();
	this->Indices.DisposeUploadBuffer();
}

D3D12_VERTEX_BUFFER_VIEW MeshGeometry::GetVertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	vertexBufferView.BufferLocation = this->Vertices.GetBufferGPU()->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = this->Vertices.GetVertexByteStride();
	vertexBufferView.SizeInBytes = this->Vertices.GetBufferByteSize();

	return vertexBufferView;
}
D3D12_INDEX_BUFFER_VIEW MeshGeometry::GetIndexBufferView() const
{
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	indexBufferView.BufferLocation = this->Indices.GetBufferGPU()->GetGPUVirtualAddress();
	indexBufferView.Format = this->Indices.GetIndexFormat();
	indexBufferView.SizeInBytes = this->Indices.GetBufferByteSize();

	return indexBufferView;
}

std::unique_ptr<MeshGeometry> MeshGeometry::LoadFromFile(const D3DBase& d3dBase, const std::string& name, const std::wstring& filename)
{
	std::ifstream fin(filename);

	if (!fin)
	{
		auto errorMessage = filename + L" not found.";
		ThrowEngineException(errorMessage.data());
	}

	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	std::vector<VertexTypes::DefaultVertexType> vertices(vcount);
	for (UINT i = 0; i < vcount; ++i)
	{
		fin >> vertices[i].Position.x >> vertices[i].Position.y >> vertices[i].Position.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	std::vector<std::int32_t> indices(3 * tcount);
	for (UINT i = 0; i < tcount; ++i)
	{
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = name;
	geo->Vertices = VertexBuffer(d3dBase, vertices.data(), vertices.size(), sizeof(VertexTypes::DefaultVertexType));
	geo->Indices = IndexBuffer(d3dBase, indices.data(), indices.size(), sizeof(std::int32_t), DXGI_FORMAT_R32_UINT);

	SubmeshGeometry submesh;
	submesh.IndexCount = static_cast<UINT>(indices.size());
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs[name] = submesh;

	return geo;
}
#include "stdafx.h"
#include "MeshGeometry.h"

using namespace GraphicsEngine;

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

void MeshGeometry::DisposeUploaders()
{
	this->Vertices.DisposeUploadBuffer();
	this->Indices.DisposeUploadBuffer();
}

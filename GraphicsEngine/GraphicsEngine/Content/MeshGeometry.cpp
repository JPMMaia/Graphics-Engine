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

void MeshGeometry::Render(ID3D12GraphicsCommandList* commandList) const
{
	// Set vertex buffer:
	auto vertexBufferView = GetVertexBufferView();
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

	// Set index buffer:
	auto indexBufferView = GetIndexBufferView();
	commandList->IASetIndexBuffer(&indexBufferView);

	// Set primitive topology:
	commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw mesh:
	auto box = DrawArgs.at("Box");
	commandList->DrawIndexedInstanced(box.IndexCount, 1, box.StartIndexLocation, box.BaseVertexLocation, 0);
}

void MeshGeometry::DisposeUploaders()
{
	this->Vertices.DisposeUploadBuffer();
	this->Indices.DisposeUploadBuffer();
}

#include "stdafx.h"
#include "Mesh.h"

#include <array>

using namespace GraphicsEngine;
using namespace std;

Mesh::Mesh() :
	m_primitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
}

void Mesh::Reset()
{
	m_indexBuffer.Reset();
	m_vertexBuffer.Reset();
}

void Mesh::Draw(ID3D11DeviceContext* d3dDeviceContext, uint32_t indexCount, uint32_t startIndexLocation) const
{
	uint32_t stride = m_vertexBuffer.GetBufferTypeSize();
	constexpr uint32_t offset = 0;
	d3dDeviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

	// Set index buffer:
	d3dDeviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Set primitive topology:
	d3dDeviceContext->IASetPrimitiveTopology(m_primitiveTopology);

	// Draw indexed instanced:
	d3dDeviceContext->DrawIndexed(indexCount, startIndexLocation, 0);
}
void Mesh::Draw(ID3D11DeviceContext* d3dDeviceContext, const InstanceBuffer& instancedData, uint32_t indexCount, uint32_t instanceCount, uint32_t startIndexLocation) const
{
	// Set vertex buffers:
	array<ID3D11Buffer*, 2> vertexBuffers = { m_vertexBuffer.Get(), instancedData.Get() };
	array<uint32_t, 2> strides = { m_vertexBuffer.GetBufferTypeSize(), instancedData.GetBufferTypeSize() };
	array<uint32_t, 2> offsets = { 0, 0 };
	d3dDeviceContext->IASetVertexBuffers(0, 2, vertexBuffers.data(), strides.data(), offsets.data());
	
	// Set index buffer:
	d3dDeviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Set primitive topology:
	d3dDeviceContext->IASetPrimitiveTopology(m_primitiveTopology);

	// Draw indexed instanced:
	d3dDeviceContext->DrawIndexedInstanced(indexCount, instanceCount, startIndexLocation, 0, 0);
}

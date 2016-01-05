#include "stdafx.h"
#include "Mesh.h"

#include <array>
#include "LightEffect.h"

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

void Mesh::Draw(ID3D11DeviceContext* d3dDeviceContext, const Buffer& instancedData, uint32_t indexCount, uint32_t instanceCount, uint32_t startIndexLocation) const
{
	// Set vertex buffers:
	array<ID3D11Buffer*, 2> vertexBuffers = { m_vertexBuffer.Get(), instancedData.Get() };
	array<uint32_t, 2> strides = { m_vertexBuffer.GetElementSize(), sizeof(LightEffect::InstanceData) };
	array<uint32_t, 2> offsets = { 0, 0 };
	d3dDeviceContext->IASetVertexBuffers(0, 2, vertexBuffers.data(), strides.data(), offsets.data());
	
	// Set index buffers:
	m_indexBuffer.Set(d3dDeviceContext);

	// Set primitive topology:
	d3dDeviceContext->IASetPrimitiveTopology(m_primitiveTopology);

	// Draw indexed instanced:
	d3dDeviceContext->DrawIndexedInstanced(indexCount, instanceCount, startIndexLocation, 0, 0);
}
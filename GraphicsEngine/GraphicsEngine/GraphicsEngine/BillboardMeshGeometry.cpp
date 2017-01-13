#include "stdafx.h"
#include "BillboardMeshGeometry.h"

using namespace GraphicsEngine;

void BillboardMeshGeometry::Update(ID3D11DeviceContext* deviceContext)
{
	if (!m_dirty)
		return;

	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		m_vertexBuffer.Map(deviceContext, D3D11_MAP_WRITE_DISCARD, &mappedResource);
		std::memcpy(mappedResource.pData, m_vertices.data(), m_vertices.size() * s_vertexBufferStride);
		m_vertexBuffer.Unmap(deviceContext);
	}

	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		m_indexBuffer.Map(deviceContext, D3D11_MAP_WRITE_DISCARD, &mappedResource);
		std::memcpy(mappedResource.pData, m_indices.data(), m_indices.size() * s_indexBufferStride);
		m_indexBuffer.Unmap(deviceContext);
	}
}

void BillboardMeshGeometry::AddInstance(ID3D11Device* device, const VertexType& instance)
{
	m_vertices.push_back(instance);
	m_indices.push_back(static_cast<IndexType>(m_indices.size()));
	RealocateBuffers(device);
}
void BillboardMeshGeometry::RemoveLastInstance()
{
	m_vertices.pop_back();
	m_indices.pop_back();
}

ID3D11Buffer* BillboardMeshGeometry::GetVertexBuffer() const
{
	return m_vertexBuffer.Get();
}
ID3D11Buffer* BillboardMeshGeometry::GetIndexBuffer() const
{
	return m_indexBuffer.Get();
}

void BillboardMeshGeometry::RealocateBuffers(ID3D11Device* device)
{
	{
		auto neededSizeForVertexBuffer = m_vertices.size() * s_vertexBufferStride;
		if (m_vertexBuffer.GetSize() < neededSizeForVertexBuffer)
		{
			// Allocate space for twice as needed:
			m_vertexBuffer.Initialize(device, static_cast<uint32_t>(neededSizeForVertexBuffer * 2), s_vertexBufferStride);
		}
	}

	{
		auto neededSizeForIndexBuffer = m_indices.size() * s_indexBufferStride;
		if (m_indexBuffer.GetSize() < neededSizeForIndexBuffer)
		{
			// Allocate space for twice as needed:
			m_indexBuffer.Initialize(device, static_cast<uint32_t>(neededSizeForIndexBuffer * 2), s_indexBufferStride);
		}
	}
}

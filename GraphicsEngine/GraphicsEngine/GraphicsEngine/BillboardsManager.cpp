#include "stdafx.h"
#include "BillboardsManager.h"

using namespace GraphicsEngine;

void BillboardsManager::AddInstance(ID3D11Device* device, const VertexTypes::BillboardVertexType& instance)
{
	m_vertices.push_back(instance);
	m_indices.push_back(m_indices.size());
	RealocateBuffers(device);
}
void BillboardsManager::RemoveLastInstance()
{
	m_vertices.pop_back();
	m_indices.pop_back();
}

void BillboardsManager::RealocateBuffers(ID3D11Device* device)
{
	{
		static constexpr auto vertexBufferStride = sizeof(VertexTypes::BillboardVertexType);
		auto neededSizeForVertexBuffer = m_vertices.size() * vertexBufferStride;
		if (m_vertexBuffer.GetSize() < neededSizeForVertexBuffer)
		{
			// Allocate space for twice as needed:
			m_vertexBuffer.Initialize(device, neededSizeForVertexBuffer * 2, vertexBufferStride);
		}
	}
	
	{
		static constexpr auto indexBufferStride = sizeof(uint32_t);
		auto neededSizeForIndexBuffer = m_indices.size() * indexBufferStride;
		if (m_indexBuffer.GetSize() < neededSizeForIndexBuffer)
		{
			// Allocate space for twice as needed:
			m_indexBuffer.Initialize(device, neededSizeForIndexBuffer * 2, indexBufferStride);
		}
	}
}

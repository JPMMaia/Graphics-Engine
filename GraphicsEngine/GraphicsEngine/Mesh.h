#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace GraphicsEngine
{
	template<class VertexType, class IndexType = uint32_t>
	class Mesh
	{
	public:
		Mesh();
		Mesh(ID3D11Device* d3dDevice, const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		void Initialize(ID3D11Device* d3dDevice, const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		void Draw(ID3D11DeviceContext* d3dDeviceContext);

	private:
		VertexBuffer<VertexType> m_vertexBuffer;
		IndexBuffer<IndexType> m_indexBuffer;
		uint32_t m_indexCount;
		D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology;
	};

	template <class VertexType, class IndexType>
	Mesh<VertexType, IndexType>::Mesh() :
		m_indexCount(0),
		m_primitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	{
	}

	template <class VertexType, class IndexType>
	Mesh<VertexType, IndexType>::Mesh(ID3D11Device* d3dDevice, const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology) :
		m_vertexBuffer(d3dDevice, vertices),
		m_indexBuffer(d3dDevice, indices),
		m_indexCount(indices.size()),
		m_primitiveTopology(primitiveTopology)
	{
	}

	template <class VertexType, class IndexType>
	void Mesh<VertexType, IndexType>::Initialize(ID3D11Device* d3dDevice, const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology)
	{
		// Initialize the vertex buffer:
		m_vertexBuffer.Initialize(d3dDevice, vertices);

		// Initialize the index buffer:
		m_indexBuffer.Initialize(d3dDevice, indices);

		// Set index count:
		m_indexCount = indices.size();

		// Set primitive topology:
		m_primitiveTopology = primitiveTopology;
	}

	template <class VertexType, class IndexType>
	void Mesh<VertexType, IndexType>::Draw(ID3D11DeviceContext* d3dDeviceContext)
	{
		// Set vertex and index buffers:
		m_vertexBuffer.Set(d3dDeviceContext);
		m_indexBuffer.Set(d3dDeviceContext);

		// Set primitive topology:
		d3dDeviceContext->IASetPrimitiveTopology(m_primitiveTopology);

		// Draw indexed:
		d3dDeviceContext->DrawIndexed(m_indexCount, 0, 0);
	}
}
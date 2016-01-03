#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace GraphicsEngine
{
	class Mesh
	{
	public:
		Mesh();

		template<class VertexType, class IndexType = uint32_t>
		Mesh(ID3D11Device* d3dDevice, const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		template<class VertexType, class IndexType = uint32_t>
		void Initialize(ID3D11Device* d3dDevice, const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		void Reset();

		void Draw(ID3D11DeviceContext* d3dDeviceContext, uint32_t indexCount, uint32_t startIndexLocation) const;

	private:
		VertexBuffer m_vertexBuffer;
		IndexBuffer m_indexBuffer;
		D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology;
	};

	template <class VertexType, class IndexType>
	Mesh::Mesh(ID3D11Device* d3dDevice, const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology) :
		m_vertexBuffer(d3dDevice, vertices),
		m_indexBuffer(d3dDevice, indices),
		m_primitiveTopology(primitiveTopology)
	{
	}

	template <class VertexType, class IndexType>
	void Mesh::Initialize(ID3D11Device* d3dDevice, const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology)
	{
		// Initialize the vertex buffer:
		m_vertexBuffer.Initialize(d3dDevice, vertices);

		// Initialize the index buffer:
		m_indexBuffer.Initialize(d3dDevice, indices);

		// Set primitive topology:
		m_primitiveTopology = primitiveTopology;
	}
}
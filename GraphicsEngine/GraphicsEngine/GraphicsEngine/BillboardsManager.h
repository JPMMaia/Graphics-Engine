#pragma once
#include "BufferTypes.h"
#include "VertexTypes.h"

namespace GraphicsEngine
{
	class BillboardsManager
	{
	public:
		void AddInstance(ID3D11Device* device, const VertexTypes::BillboardVertexType& instance);
		void RemoveLastInstance();

	private:
		void RealocateBuffers(ID3D11Device* device);
		
	private:
		InstanceBuffer m_vertexBuffer;
		InstanceBuffer m_indexBuffer;
		std::vector<VertexTypes::BillboardVertexType> m_vertices;
		std::vector<uint32_t> m_indices;
	};
}

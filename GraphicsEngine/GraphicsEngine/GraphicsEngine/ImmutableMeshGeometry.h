#pragma once

#include "MeshGeometry.h"

namespace GraphicsEngine
{
	class ImmutableMeshGeometry : public MeshGeometry
	{
	public:
		template<typename BufferType>
		void CreateVertexBuffer(ID3D11Device* device, const std::vector<BufferType>& data)
		{
			m_vertexBuffer.Initialize(device, data);
		}

		template<typename BufferType>
		void CreateIndexBuffer(ID3D11Device* device, const std::vector<BufferType>& data)
		{
			m_indexBuffer.Initialize(device, data);
		}

		ID3D11Buffer* GetVertexBuffer() const override;
		ID3D11Buffer* GetIndexBuffer() const override;

	private:
		VertexBuffer m_vertexBuffer;
		IndexBuffer m_indexBuffer;
	};
}

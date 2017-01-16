#pragma once

#include "MeshGeometry.h"

namespace GraphicsEngine
{
	class ImmutableMeshGeometry : public MeshGeometry
	{
	public:
		template<typename BufferType>
		void CreateVertexBuffer(ID3D11Device* device, const std::vector<BufferType>& data, D3D_PRIMITIVE_TOPOLOGY primitiveType)
		{
			m_stride = sizeof(BufferType);
			m_offset = 0;
			m_primitiveType = primitiveType;
			m_vertexBuffer.Initialize(device, data);
		}

		template<typename BufferType>
		void CreateIndexBuffer(ID3D11Device* device, const std::vector<BufferType>& data, DXGI_FORMAT indexFormat)
		{
			m_indexFormat = indexFormat;
			m_indexBuffer.Initialize(device, data);
		}

		ID3D11Buffer* GetVertexBuffer() const override;
		ID3D11Buffer* GetIndexBuffer() const override;
		UINT GetStride() const override;
		UINT GetOffset() const override;
		DXGI_FORMAT GetIndexFormat() const override;
		D3D_PRIMITIVE_TOPOLOGY GetPrimitiveType() const override;

	private:
		VertexBuffer m_vertexBuffer;
		IndexBuffer m_indexBuffer;
		UINT m_stride = 0;
		UINT m_offset = 0;
		DXGI_FORMAT m_indexFormat = DXGI_FORMAT_R32_UINT;
		D3D_PRIMITIVE_TOPOLOGY m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	};
}

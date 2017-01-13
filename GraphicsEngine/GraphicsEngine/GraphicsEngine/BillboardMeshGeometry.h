#pragma once

#include "MeshGeometry.h"
#include "VertexTypes.h"

namespace GraphicsEngine
{
	class BillboardMeshGeometry : public MeshGeometry
	{
	public:
		using VertexType = VertexTypes::BillboardVertexType;
		using IndexType = uint32_t;

	public:

		void Update(ID3D11DeviceContext* deviceContext);

		void AddInstance(ID3D11Device* device, const VertexType& instance);
		void AddInstances(ID3D11Device* device, const std::vector<VertexType>& instances);
		void RemoveLastInstance();
		
		size_t GetInstanceCount() const;
		ID3D11Buffer* GetVertexBuffer() const override;
		ID3D11Buffer* GetIndexBuffer() const override;
		UINT GetStride() const override;
		UINT GetOffset() const override;
		DXGI_FORMAT GetIndexFormat() const override;
		D3D_PRIMITIVE_TOPOLOGY GetPrimitiveType() const override;

	private:
		void RealocateBuffers(ID3D11Device* device);

	private:
		static constexpr auto s_vertexBufferStride = sizeof(VertexType);
		static constexpr auto s_indexBufferStride = sizeof(IndexType);

		InstanceBuffer m_vertexBuffer;
		InstanceBuffer m_indexBuffer;
		std::vector<VertexType> m_vertices;
		std::vector<IndexType> m_indices;
		bool m_dirty = true;
	};
}

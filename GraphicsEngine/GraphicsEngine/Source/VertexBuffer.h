#pragma once

#include "Buffer.h"

namespace GraphicsEngine
{
	class VertexBuffer
	{
	public:
		VertexBuffer();

		template<class VertexType>
		VertexBuffer(ID3D11Device* d3dDevice, const std::vector<VertexType>& vertices);

		template<class VertexType>
		void Initialize(ID3D11Device* d3dDevice, const std::vector<VertexType>& vertices);
		void Reset();

		void Set(ID3D11DeviceContext* d3dDeviceContext) const;

	private:
		Buffer m_vertexBuffer;
		uint32_t m_stride;
	};

	template <class VertexType>
	VertexBuffer::VertexBuffer(ID3D11Device* d3dDevice, const std::vector<VertexType>& vertices) :
		m_vertexBuffer(d3dDevice, vertices, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE),
		m_stride(sizeof(VertexType))
	{
	}

	template <class VertexType>
	void VertexBuffer::Initialize(ID3D11Device* d3dDevice, const std::vector<VertexType>& vertices)
	{
		m_vertexBuffer.Initialize(d3dDevice, vertices, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
		m_stride = sizeof(VertexType);
	}
}
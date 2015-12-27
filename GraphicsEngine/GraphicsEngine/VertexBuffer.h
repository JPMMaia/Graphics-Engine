#pragma once

#include "Buffer.h"

namespace GraphicsEngine
{
	template<class VertexType>
	class VertexBuffer
	{
	public:
		VertexBuffer(ID3D11Device* d3dDevice, const std::vector<VertexType>& vertices);

		void Set(ID3D11DeviceContext* d3dDeviceContext);

	private:
		Buffer<VertexType> m_vertexBuffer;
	};

	template <class VertexType>
	VertexBuffer<VertexType>::VertexBuffer(ID3D11Device* d3dDevice, const std::vector<VertexType>& vertices) :
		m_vertexBuffer(d3dDevice, vertices, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE)
	{
	}

	template <class VertexType>
	void VertexBuffer<VertexType>::Set(ID3D11DeviceContext* d3dDeviceContext)
	{
		uint32_t stride = sizeof(VertexType);
		uint32_t offset = 0;
		d3dDeviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	}
}
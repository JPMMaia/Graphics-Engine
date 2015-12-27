#pragma once

#include "Buffer.h"

#include <cstdint>

namespace GraphicsEngine
{
	template<class IndexType>
	class IndexBuffer
	{
	public:
		IndexBuffer();
		IndexBuffer(ID3D11Device* d3dDevice, const std::vector<IndexType>& indices);

		void Initialize(ID3D11Device* d3dDevice, const std::vector<IndexType>& indices);
		void Shutdown();

		void Set(ID3D11DeviceContext* d3dDeviceContext) const;

	private:
		Buffer<IndexType> m_indexBuffer;
	};

	template <class IndexType>
	IndexBuffer<IndexType>::IndexBuffer()
	{
	}

	template <class IndexType>
	IndexBuffer<IndexType>::IndexBuffer(ID3D11Device* d3dDevice, const std::vector<IndexType>& indices) :
		m_indexBuffer(d3dDevice, indices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE)
	{
	}

	template <class IndexType>
	void IndexBuffer<IndexType>::Initialize(ID3D11Device* d3dDevice, const std::vector<IndexType>& indices)
	{
		m_indexBuffer.Initialize(d3dDevice, indices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
	}

	template <class IndexType>
	void IndexBuffer<IndexType>::Shutdown()
	{
		m_indexBuffer.Shutdown();
	}

	template <>
	inline void IndexBuffer<uint32_t>::Set(ID3D11DeviceContext* d3dDeviceContext) const
	{
		d3dDeviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	template <>
	inline void IndexBuffer<uint16_t>::Set(ID3D11DeviceContext* d3dDeviceContext) const
	{
		d3dDeviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	}
}

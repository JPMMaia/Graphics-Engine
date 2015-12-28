#pragma once

#include "Buffer.h"

namespace GraphicsEngine
{
	class IndexBuffer
	{
	public:
		IndexBuffer();
		IndexBuffer(ID3D11Device* d3dDevice, const std::vector<uint16_t>& indices);
		IndexBuffer(ID3D11Device* d3dDevice, const std::vector<uint32_t>& indices);

		void Initialize(ID3D11Device* d3dDevice, const std::vector<uint16_t>& indices);
		void Initialize(ID3D11Device* d3dDevice, const std::vector<uint32_t>& indices);

		void Reset();

		void Set(ID3D11DeviceContext* d3dDeviceContext) const;

	private:
		template<class IndexType>
		void InitializeBuffer(ID3D11Device* d3dDevice, const std::vector<IndexType>& indices);

	private:
		Buffer m_indexBuffer;
		DXGI_FORMAT m_format;
	};

	template <class IndexType>
	void IndexBuffer::InitializeBuffer(ID3D11Device* d3dDevice, const std::vector<IndexType>& indices)
	{
		m_indexBuffer.Initialize(d3dDevice, indices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
	}
}

#pragma once

#include <d3d11_2.h>
#include <vector>
#include <wrl/client.h>

#include "Common/Helpers.h"

namespace GraphicsEngine
{
	template<D3D11_BIND_FLAG BIND_FLAG, D3D11_USAGE USAGE_FLAG, uint32_t CPU_ACCESS_FLAG>
	class Buffer
	{
	public:
		Buffer() = default;

		template<typename BufferType>
		Buffer(ID3D11Device* d3dDevice, const std::vector<BufferType>& initialData)
		{
			Initialize(d3dDevice, initialData);
		}

		Buffer(ID3D11Device* d3dDevice, uint32_t bufferSize, uint32_t stride)
		{
			Initialize(d3dDevice, bufferSize, stride);
		}

		template<typename BufferType>
		void Initialize(ID3D11Device* d3dDevice, const std::vector<BufferType>& initialData)
		{
			// Specify the initial data:
			D3D11_SUBRESOURCE_DATA subresourceData;
			subresourceData.pSysMem = &initialData[0];

			// Create buffer with the specified initial data:
			auto stride = static_cast<uint32_t>(sizeof(BufferType));
			Initialize(d3dDevice, static_cast<uint32_t>(stride * initialData.size()), stride, &subresourceData);
		}
		
		void Initialize(ID3D11Device* d3dDevice, uint32_t bufferSize, uint32_t stride)
		{
			// Create empty buffer with the specified size:
			Initialize(d3dDevice, bufferSize, stride, nullptr);
		}

		void Reset()
		{
			m_buffer.Reset();
		}

		template<typename = std::enable_if_t<USAGE_FLAG == D3D11_USAGE_DYNAMIC && CPU_ACCESS_FLAG == D3D11_CPU_ACCESS_WRITE>>
		void CopyData(ID3D11DeviceContext* d3dDeviceContext, const void* bufferData, uint32_t bufferSize) const
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource = {};

			// Disable GPU access to the buffer data:
			d3dDeviceContext->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			// Update the dynamic buffer:
			memcpy_s(mappedResource.pData, bufferSize, bufferData, bufferSize);

			// Reenable GPU access to the buffer data:
			d3dDeviceContext->Unmap(m_buffer.Get(), 0);
		}

		template<typename = std::enable_if_t<(USAGE_FLAG == D3D11_USAGE_DYNAMIC && CPU_ACCESS_FLAG == D3D11_CPU_ACCESS_WRITE) || (USAGE_FLAG == D3D11_USAGE_STAGING && CPU_ACCESS_FLAG == D3D11_CPU_ACCESS_READ)>>
		void Map(ID3D11DeviceContext* d3dDeviceContext, D3D11_MAP mapType, D3D11_MAPPED_SUBRESOURCE* mappedResource) const
		{
			d3dDeviceContext->Map(m_buffer.Get(), 0, mapType, 0, mappedResource);
		}
		
		template<typename = std::enable_if_t<(USAGE_FLAG == D3D11_USAGE_DYNAMIC && CPU_ACCESS_FLAG == D3D11_CPU_ACCESS_WRITE) || (USAGE_FLAG == D3D11_USAGE_STAGING && CPU_ACCESS_FLAG == D3D11_CPU_ACCESS_READ)>>
		void Unmap(ID3D11DeviceContext* d3dDeviceContext) const
		{
			d3dDeviceContext->Unmap(m_buffer.Get(), 0);
		}

		template<typename = std::enable_if_t<USAGE_FLAG == D3D11_USAGE_DEFAULT>>
		void Update(ID3D11DeviceContext1* d3dDeviceContext, const void* bufferData, uint32_t bufferSize) const
		{
			// Copy data from memory to a subresource created in non-mappable memory:
			d3dDeviceContext->UpdateSubresource1(
				m_buffer.Get(),
				0,
				nullptr,
				&bufferData,
				0,
				0,
				0
			);
		}

		ID3D11Buffer* Get() const
		{
			return m_buffer.Get();
		}
		ID3D11Buffer* const* GetAddressOf() const
		{
			return m_buffer.GetAddressOf();
		}
		uint32_t GetSize() const
		{
			return m_size;
		}
		uint32_t GetStride() const
		{
			return m_stride;
		}

	private:
		void Initialize(ID3D11Device* d3dDevice, uint32_t bufferSize, uint32_t stride, const D3D11_SUBRESOURCE_DATA* initialData)
		{
			m_stride = stride;
			m_size = bufferSize;

			CreateBuffer(d3dDevice, bufferSize, initialData);
		}
		void CreateBuffer(ID3D11Device* d3dDevice, uint32_t bufferSize, const D3D11_SUBRESOURCE_DATA* initialData)
		{
			// Create buffer description:
			D3D11_BUFFER_DESC bufferDesc = CD3D11_BUFFER_DESC(
				bufferSize,									// Size of buffer
				BIND_FLAG,									// Bind to vertex buffer flag
				USAGE_FLAG,									// Vertex buffer should be immutable
				CPU_ACCESS_FLAG
			);

			// Create buffer:
			Common::ThrowIfFailed(
				d3dDevice->CreateBuffer(&bufferDesc, initialData, m_buffer.GetAddressOf())
			);
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
		uint32_t m_size = 0;
		uint32_t m_stride = 0;
	};
}
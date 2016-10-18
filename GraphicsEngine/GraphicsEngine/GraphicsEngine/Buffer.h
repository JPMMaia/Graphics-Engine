#pragma once

#include <d3d11_2.h>
#include <vector>
#include <wrl/client.h>

namespace GraphicsEngine
{
	template<D3D11_BIND_FLAG BIND_FLAG, D3D11_USAGE USAGE_FLAG, uint32_t CPU_ACCESS_FLAG>
	class Buffer
	{
	public:
		Buffer();

		template<typename BufferType>
		Buffer(ID3D11Device* d3dDevice, const std::vector<BufferType>& initialData);

		template<typename BufferType>
		Buffer(ID3D11Device* d3dDevice, uint32_t bufferSize);

		template<typename BufferType>
		void Initialize(ID3D11Device* d3dDevice, const std::vector<BufferType>& initialData);
		
		template<typename BufferType>
		void Initialize(ID3D11Device* d3dDevice, uint32_t bufferSize);

		void Reset();

		template<typename = std::enable_if_t<USAGE_FLAG == D3D11_USAGE_DYNAMIC && CPU_ACCESS_FLAG == D3D11_CPU_ACCESS_WRITE>>
		void Map(ID3D11DeviceContext1* d3dDeviceContext, const void* bufferData, uint32_t bufferSize) const;

		template<typename = std::enable_if_t<USAGE_FLAG == D3D11_USAGE_DEFAULT>>
		void Update(ID3D11DeviceContext1* d3dDeviceContext, const void* bufferData, uint32_t bufferSize) const;

		ID3D11Buffer* Get() const;
		ID3D11Buffer* const* GetAddressOf() const;
		uint32_t GetBufferTypeSize() const;

	private:
		template<typename BufferType>
		void Initialize(ID3D11Device* d3dDevice, uint32_t bufferSize, const D3D11_SUBRESOURCE_DATA* initialData);
		void CreateBuffer(ID3D11Device* d3dDevice, uint32_t bufferSize, const D3D11_SUBRESOURCE_DATA* initialData);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
		uint32_t m_bufferTypeSize;
	};

	template <D3D11_BIND_FLAG BIND_FLAG, D3D11_USAGE USAGE_FLAG, uint32_t CPU_ACCESS_FLAG>
	Buffer<BIND_FLAG, USAGE_FLAG, CPU_ACCESS_FLAG>::Buffer() :
		m_bufferTypeSize(0)
	{
	}

	template <D3D11_BIND_FLAG BIND_FLAG, D3D11_USAGE USAGE_FLAG, uint32_t CPU_ACCESS_FLAG>
	template<typename BufferType>
	Buffer<BIND_FLAG, USAGE_FLAG, CPU_ACCESS_FLAG>::Buffer(ID3D11Device* d3dDevice, const std::vector<BufferType>& initialData)
	{
		Initialize(d3dDevice, initialData);
	}

	template <D3D11_BIND_FLAG BIND_FLAG, D3D11_USAGE USAGE_FLAG, uint32_t CPU_ACCESS_FLAG>
	template<typename BufferType>
	Buffer<BIND_FLAG, USAGE_FLAG, CPU_ACCESS_FLAG>::Buffer(ID3D11Device* d3dDevice, uint32_t bufferSize)
	{
		Initialize<BufferType>(d3dDevice, bufferSize);
	}

	template <D3D11_BIND_FLAG BIND_FLAG, D3D11_USAGE USAGE_FLAG, uint32_t CPU_ACCESS_FLAG>
	template<typename BufferType>
	void Buffer<BIND_FLAG, USAGE_FLAG, CPU_ACCESS_FLAG>::Initialize(ID3D11Device* d3dDevice, const std::vector<BufferType>& initialData)
	{
		// Specify the initial data:
		D3D11_SUBRESOURCE_DATA subresourceData;
		subresourceData.pSysMem = &initialData[0];

		// Create buffer with the specified initial data:
		Initialize<BufferType>(d3dDevice, sizeof(BufferType) * initialData.size(), &subresourceData);
	}

	template <D3D11_BIND_FLAG BIND_FLAG, D3D11_USAGE USAGE_FLAG, uint32_t CPU_ACCESS_FLAG>
	template <typename BufferType>
	void Buffer<BIND_FLAG, USAGE_FLAG, CPU_ACCESS_FLAG>::Initialize(ID3D11Device* d3dDevice, uint32_t bufferSize)
	{
		// Create empty buffer with the specified size:
		Initialize<BufferType>(d3dDevice, bufferSize, nullptr);
	}

	template <D3D11_BIND_FLAG BIND_FLAG, D3D11_USAGE USAGE_FLAG, uint32_t CPU_ACCESS_FLAG>
	void Buffer<BIND_FLAG, USAGE_FLAG, CPU_ACCESS_FLAG>::Reset()
	{
		m_buffer.Reset();
	}

	template <D3D11_BIND_FLAG BIND_FLAG, D3D11_USAGE USAGE_FLAG, uint32_t CPU_ACCESS_FLAG>
	template <typename>
	void Buffer<BIND_FLAG, USAGE_FLAG, CPU_ACCESS_FLAG>::Map(ID3D11DeviceContext1* d3dDeviceContext, const void* bufferData, uint32_t bufferSize) const
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};

		// Disable GPU access to the buffer data:
		d3dDeviceContext->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		// Update the dynamic buffer:
		memcpy_s(mappedResource.pData, bufferSize, bufferData, bufferSize);

		// Reenable GPU access to the buffer data:
		d3dDeviceContext->Unmap(m_buffer.Get(), 0);
	}

	template <D3D11_BIND_FLAG BIND_FLAG, D3D11_USAGE USAGE_FLAG, uint32_t CPU_ACCESS_FLAG>
	template <typename>
	void Buffer<BIND_FLAG, USAGE_FLAG, CPU_ACCESS_FLAG>::Update(ID3D11DeviceContext1* d3dDeviceContext, const void* bufferData, uint32_t bufferSize) const
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

	template <D3D11_BIND_FLAG BIND_FLAG, D3D11_USAGE USAGE_FLAG, uint32_t CPU_ACCESS_FLAG>
	ID3D11Buffer* Buffer<BIND_FLAG, USAGE_FLAG, CPU_ACCESS_FLAG>::Get() const
	{
		return m_buffer.Get();
	}

	template <D3D11_BIND_FLAG BIND_FLAG, D3D11_USAGE USAGE_FLAG, uint32_t CPU_ACCESS_FLAG>
	ID3D11Buffer* const* Buffer<BIND_FLAG, USAGE_FLAG, CPU_ACCESS_FLAG>::GetAddressOf() const
	{
		return m_buffer.GetAddressOf();
	}

	template <D3D11_BIND_FLAG BIND_FLAG, D3D11_USAGE USAGE_FLAG, uint32_t CPU_ACCESS_FLAG>
	uint32_t Buffer<BIND_FLAG, USAGE_FLAG, CPU_ACCESS_FLAG>::GetBufferTypeSize() const
	{
		return m_bufferTypeSize;
	}

	template <D3D11_BIND_FLAG BIND_FLAG, D3D11_USAGE USAGE_FLAG, uint32_t CPU_ACCESS_FLAG>
	template <typename BufferType>
	void Buffer<BIND_FLAG, USAGE_FLAG, CPU_ACCESS_FLAG>::Initialize(ID3D11Device* d3dDevice, uint32_t bufferSize, const D3D11_SUBRESOURCE_DATA* initialData)
	{
		m_bufferTypeSize = sizeof(BufferType);

		CreateBuffer(d3dDevice, bufferSize, initialData);
	}

	template <D3D11_BIND_FLAG BIND_FLAG, D3D11_USAGE USAGE_FLAG, uint32_t CPU_ACCESS_FLAG>
	void Buffer<BIND_FLAG, USAGE_FLAG, CPU_ACCESS_FLAG>::CreateBuffer(ID3D11Device* d3dDevice, uint32_t bufferSize, const D3D11_SUBRESOURCE_DATA* initialData)
	{
		// Create buffer description:
		D3D11_BUFFER_DESC bufferDesc = CD3D11_BUFFER_DESC(
			bufferSize,									// Size of buffer
			BIND_FLAG,									// Bind to vertex buffer flag
			USAGE_FLAG,									// Vertex buffer should be immutable
			CPU_ACCESS_FLAG
			);

		// Create buffer:
		DX::ThrowIfFailed(
			d3dDevice->CreateBuffer(&bufferDesc, initialData, m_buffer.GetAddressOf())
			);
	}
}
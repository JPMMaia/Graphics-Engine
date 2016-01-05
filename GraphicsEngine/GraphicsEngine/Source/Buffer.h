#pragma once

#include <vector>

namespace GraphicsEngine
{
	class Buffer
	{
	public:
		Buffer();

		template<typename BufferType>
		Buffer(ID3D11Device* d3dDevice, const std::vector<BufferType>& initialData, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage, uint32_t cpuAccessFlags = 0);
		Buffer(ID3D11Device* d3dDevice, uint32_t bufferSize, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage, uint32_t cpuAccessFlags = 0);

		template<typename BufferType>
		void Initialize(ID3D11Device* d3dDevice, const std::vector<BufferType>& initialData, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage, uint32_t cpuAccessFlags = 0);
		void Initialize(ID3D11Device* d3dDevice, uint32_t bufferSize, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage, uint32_t cpuAccessFlags = 0);
		void Reset();

		template<typename BufferType, size_t BUFFER_SIZE>
		void Update(ID3D11DeviceContext1* d3dDeviceContext, const std::array<BufferType, BUFFER_SIZE>& bufferData) const;

		ID3D11Buffer* Get() const;
		ID3D11Buffer* const* GetAddressOf() const;
		uint32_t GetElementSize() const;

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
		uint32_t m_elementSize;
	};

	template <class BufferType>
	Buffer::Buffer(ID3D11Device* d3dDevice, const std::vector<BufferType>& initialData, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage, uint32_t cpuAccessFlags)
	{
		Initialize(d3dDevice, initialData, bindFlags, usage);
	}

	template <class BufferType>
	void Buffer::Initialize(ID3D11Device* d3dDevice, const std::vector<BufferType>& initialData, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage, uint32_t cpuAccessFlags)
	{
		m_elementSize = sizeof(BufferType);

		// Create buffer description:
		D3D11_BUFFER_DESC bufferDesc = CD3D11_BUFFER_DESC(
			m_elementSize * initialData.size(),										// Size of buffer
			bindFlags,									// Bind to vertex buffer flag
			usage,										// Vertex buffer should be immutable
			cpuAccessFlags
			);
		
		// Specify the initial data:
		D3D11_SUBRESOURCE_DATA subresourceData;
		subresourceData.pSysMem = &initialData[0];

		// Create buffer:
		DX::ThrowIfFailed(
			d3dDevice->CreateBuffer(&bufferDesc, &subresourceData, m_buffer.GetAddressOf())
			);
	}

	template <typename BufferType, size_t BUFFER_SIZE>
	void Buffer::Update(ID3D11DeviceContext1* d3dDeviceContext, const std::array<BufferType, BUFFER_SIZE>& bufferData) const
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};

		// Disable GPU access to the buffer data:
		d3dDeviceContext->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		// Update the dynamic buffer:
		uint32_t size = bufferData.size() * sizeof(BufferType);
		memcpy_s(mappedResource.pData, size, bufferData.data(), size);

		// Reenable GPU access to the buffer data:
		d3dDeviceContext->Unmap(m_buffer.Get(), 0);
	}
}
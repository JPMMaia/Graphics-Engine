#pragma once

#include <vector>

namespace GraphicsEngine
{
	class Buffer
	{
	public:
		Buffer();

		template<class BufferType>
		Buffer(ID3D11Device* d3dDevice, const std::vector<BufferType>& initialData, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage);
		Buffer(ID3D11Device* d3dDevice, uint32_t bufferSize, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage);

		template<class BufferType>
		void Initialize(ID3D11Device* d3dDevice, const std::vector<BufferType>& initialData, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage);
		void Initialize(ID3D11Device* d3dDevice, uint32_t bufferSize, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage);
		void Reset();

		ID3D11Buffer* Get() const;
		ID3D11Buffer** GetAddressOf();

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	};

	template <class BufferType>
	Buffer::Buffer(ID3D11Device* d3dDevice, const std::vector<BufferType>& initialData, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage)
	{
		Initialize(d3dDevice, initialData, bindFlags, usage);
	}

	template <class BufferType>
	void Buffer::Initialize(ID3D11Device* d3dDevice, const std::vector<BufferType>& initialData, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage)
	{
		// Create buffer description:
		D3D11_BUFFER_DESC bufferDesc = CD3D11_BUFFER_DESC(
			sizeof(BufferType) * initialData.size(),	// Size of buffer
			bindFlags,									// Bind to vertex buffer flag
			usage										// Vertex buffer should be immutable
			);

		// Specify the initial data:
		D3D11_SUBRESOURCE_DATA subresourceData;
		subresourceData.pSysMem = &initialData[0];

		// Create buffer:
		DX::ThrowIfFailed(
			d3dDevice->CreateBuffer(&bufferDesc, &subresourceData, m_buffer.GetAddressOf())
			);
	}
}

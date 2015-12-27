#pragma once

namespace GraphicsEngine
{
	template<class BufferType>
	class Buffer
	{
	public:
		Buffer(ID3D11Device* d3dDevice, const std::vector<BufferType>& initialData, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage);

		ID3D11Buffer* Get() const;
		ID3D11Buffer** GetAddressOf();

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	};

	template <class BufferType>
	Buffer<BufferType>::Buffer(ID3D11Device* d3dDevice, const std::vector<BufferType>& initialData, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage)
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

	template <class BufferType>
	ID3D11Buffer* Buffer<BufferType>::Get() const
	{
		return m_buffer.Get();
	}

	template <class BufferType>
	ID3D11Buffer** Buffer<BufferType>::GetAddressOf()
	{
		return m_buffer.GetAddressOf();
	}
}

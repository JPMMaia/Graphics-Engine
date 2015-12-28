#include "stdafx.h"
#include "Buffer.h"

using namespace GraphicsEngine;

Buffer::Buffer()
{
}

Buffer::Buffer(ID3D11Device* d3dDevice, uint32_t bufferSize, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage, uint32_t cpuAccessFlags)
{
	Initialize(d3dDevice, bufferSize, bindFlags, usage, cpuAccessFlags);
}

void Buffer::Initialize(ID3D11Device* d3dDevice, uint32_t bufferSize, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage, uint32_t cpuAccessFlags)
{
	// Create buffer description:
	D3D11_BUFFER_DESC bufferDesc = CD3D11_BUFFER_DESC(
		bufferSize,									// Size of buffer
		bindFlags,									// Bind to vertex buffer flag
		usage,										// Vertex buffer should be immutable
		cpuAccessFlags
		);

	// Create buffer:
	DX::ThrowIfFailed(
		d3dDevice->CreateBuffer(&bufferDesc, nullptr, m_buffer.GetAddressOf())
		);
}

void Buffer::Reset()
{
	m_buffer.Reset();
}

ID3D11Buffer* Buffer::Get() const
{
	return m_buffer.Get();
}

ID3D11Buffer** Buffer::GetAddressOf()
{
	return m_buffer.GetAddressOf();
}
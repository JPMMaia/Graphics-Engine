#include "stdafx.h"
#include "BaseGeometryBuffer.h"
#include "D3DBase.h"

#include <d3dcompiler.h>

using namespace GraphicsEngine;

BaseGeometryBuffer::BaseGeometryBuffer(const D3DBase& d3dBase, const void* data, uint32_t count, uint32_t elementByteSize) :
	m_bufferByteSize(count * elementByteSize)
{
	// Create CPU buffer and copy vertex data:
	DX::ThrowIfFailed(
		D3DCreateBlob(m_bufferByteSize, m_bufferCPU.GetAddressOf())
		);
	CopyMemory(m_bufferCPU->GetBufferPointer(), data, m_bufferByteSize);

	// Create GPU buffer:
	m_bufferGPU = DX::CreateDefaultBuffer(d3dBase.GetDevice(), d3dBase.GetCommandList(), data, m_bufferByteSize, m_uploadBuffer);
}

void BaseGeometryBuffer::DisposeUploadBuffer()
{
	m_uploadBuffer = nullptr;
}
ID3DBlob* BaseGeometryBuffer::GetBufferCPU() const
{
	return m_bufferCPU.Get();
}
ID3D12Resource* BaseGeometryBuffer::GetBufferGPU() const
{
	return m_bufferGPU.Get();
}
uint32_t BaseGeometryBuffer::GetBufferByteSize() const
{
	return m_bufferByteSize;
}

#include "stdafx.h"
#include "IndexBuffer.h"

using namespace GraphicsEngine;

IndexBuffer::IndexBuffer() :
	m_format(DXGI_FORMAT_R32_UINT)
{
}

IndexBuffer::IndexBuffer(ID3D11Device* d3dDevice, const std::vector<uint16_t>& indices) :
	m_indexBuffer(d3dDevice, indices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE),
	m_format(DXGI_FORMAT_R16_UINT)
{
}

IndexBuffer::IndexBuffer(ID3D11Device* d3dDevice, const std::vector<uint32_t>& indices) :
	m_indexBuffer(d3dDevice, indices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE),
	m_format(DXGI_FORMAT_R32_UINT)
{
}

void IndexBuffer::Initialize(ID3D11Device* d3dDevice, const std::vector<uint16_t>& indices)
{
	InitializeBuffer(d3dDevice, indices);
	m_format = DXGI_FORMAT_R16_UINT;
}
void IndexBuffer::Initialize(ID3D11Device* d3dDevice, const std::vector<uint32_t>& indices)
{
	InitializeBuffer(d3dDevice, indices);
	m_format = DXGI_FORMAT_R32_UINT;
}

void IndexBuffer::Reset()
{
	m_indexBuffer.Reset();
}

void IndexBuffer::Set(ID3D11DeviceContext* d3dDeviceContext) const
{
	d3dDeviceContext->IASetIndexBuffer(m_indexBuffer.Get(), m_format, 0);
}
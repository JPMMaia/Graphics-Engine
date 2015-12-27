#include "stdafx.h"
#include "IndexBuffer.h"

using namespace GraphicsEngine;

IndexBuffer::IndexBuffer(ID3D11Device* d3dDevice, const std::vector<uint32_t>& indices) :
	m_indexBuffer(d3dDevice, indices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE)
{
}

void IndexBuffer::Set(ID3D11DeviceContext* d3dDeviceContext) const
{
	d3dDeviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

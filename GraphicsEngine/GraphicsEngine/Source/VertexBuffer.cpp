#include "stdafx.h"
#include "VertexBuffer.h"

using namespace GraphicsEngine;

VertexBuffer::VertexBuffer() :
	m_stride(0)
{
}

void VertexBuffer::Reset()
{
	m_vertexBuffer.Reset();
}

void VertexBuffer::Set(ID3D11DeviceContext* d3dDeviceContext) const
{
	uint32_t offset = 0;
	d3dDeviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &m_stride, &offset);
}

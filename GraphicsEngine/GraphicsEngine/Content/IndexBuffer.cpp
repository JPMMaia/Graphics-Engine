#include "stdafx.h"
#include "IndexBuffer.h"

using namespace GraphicsEngine;

IndexBuffer::IndexBuffer(const D3DBase& d3dBase, const void* indices, uint32_t indexCount, uint32_t indexByteSize, DXGI_FORMAT indexFormat) :
	m_indexFormat(indexFormat),
	BaseGeometryBuffer(d3dBase, indices, indexCount, indexByteSize)
{
}

DXGI_FORMAT IndexBuffer::GetIndexFormat() const
{
	return m_indexFormat;
}

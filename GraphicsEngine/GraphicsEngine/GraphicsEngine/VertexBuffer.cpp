#include "stdafx.h"
#include "VertexBuffer.h"

using namespace GraphicsEngine;

VertexBuffer::VertexBuffer(const D3DBase& d3dBase, const void* vertices, size_t vertexCount, size_t vertexByteSize) :
	m_vertexByteStride(static_cast<uint32_t>(vertexByteSize)),
	BaseGeometryBuffer(d3dBase, vertices, vertexCount, vertexByteSize)
{
}

uint32_t VertexBuffer::GetVertexByteStride() const
{
	return m_vertexByteStride;
}

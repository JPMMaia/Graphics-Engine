#include "stdafx.h"
#include "VertexBuffer.h"

using namespace GraphicsEngine;

VertexBuffer::VertexBuffer(const D3DBase& d3dBase, const void* vertices, uint32_t vertexCount, uint32_t vertexByteSize) :
	m_vertexByteStride(vertexByteSize),
	BaseGeometryBuffer(d3dBase, vertices, vertexCount, vertexByteSize)
{
}

uint32_t VertexBuffer::GetVertexByteStride() const
{
	return m_vertexByteStride;
}

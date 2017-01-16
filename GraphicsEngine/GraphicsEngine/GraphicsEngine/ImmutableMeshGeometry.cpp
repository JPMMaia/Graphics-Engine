#include "stdafx.h"
#include "ImmutableMeshGeometry.h"

using namespace GraphicsEngine;

ID3D11Buffer* ImmutableMeshGeometry::GetVertexBuffer() const
{
	return m_vertexBuffer.Get();
}
ID3D11Buffer* ImmutableMeshGeometry::GetIndexBuffer() const
{
	return m_indexBuffer.Get();
}
UINT ImmutableMeshGeometry::GetStride() const
{
	return m_stride;
}
UINT ImmutableMeshGeometry::GetOffset() const
{
	return m_offset;
}
DXGI_FORMAT ImmutableMeshGeometry::GetIndexFormat() const
{
	return m_indexFormat;
}
D3D_PRIMITIVE_TOPOLOGY ImmutableMeshGeometry::GetPrimitiveType() const
{
	return m_primitiveType;
}

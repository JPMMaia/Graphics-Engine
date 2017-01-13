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

#include "stdafx.h"
#include "Mesh.h"

using namespace GraphicsEngine;

Mesh::Mesh() :
	m_indexCount(0),
	m_primitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
}

void Mesh::Reset()
{
	m_indexCount = 0;
	m_indexBuffer.Reset();
	m_vertexBuffer.Reset();
}

void Mesh::Draw(ID3D11DeviceContext* d3dDeviceContext) const
{
	// Set vertex and index buffers:
	m_vertexBuffer.Set(d3dDeviceContext);
	m_indexBuffer.Set(d3dDeviceContext);

	// Set primitive topology:
	d3dDeviceContext->IASetPrimitiveTopology(m_primitiveTopology);

	// Draw indexed:
	d3dDeviceContext->DrawIndexed(m_indexCount, 0, 0);
}

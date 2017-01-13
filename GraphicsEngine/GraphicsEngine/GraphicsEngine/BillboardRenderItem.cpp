#include "stdafx.h"
#include "BillboardRenderItem.h"

using namespace GraphicsEngine;

void BillboardRenderItem::Render(ID3D11DeviceContext* deviceContext) const
{
	RenderNonInstanced(deviceContext);
}
void BillboardRenderItem::RenderNonInstanced(ID3D11DeviceContext* deviceContext) const
{
	std::array<ID3D11Buffer*, 1> vertexBuffers = { m_mesh->GetVertexBuffer() };
	std::array<UINT, 1> strides = { m_mesh->GetStride() };
	std::array<UINT, 1> offsets = { m_mesh->GetOffset() };
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffers.data(), strides.data(), offsets.data());
	deviceContext->IASetIndexBuffer(m_mesh->GetIndexBuffer(), m_mesh->GetIndexFormat(), 0);
	deviceContext->IASetPrimitiveTopology(m_mesh->GetPrimitiveType());

	deviceContext->DrawIndexed(static_cast<UINT>(m_mesh->GetInstanceCount()), 0, 0);
}

void BillboardRenderItem::Update(ID3D11DeviceContext* deviceContext) const
{
	m_mesh->Update(deviceContext);
}

void BillboardRenderItem::AddInstance(ID3D11Device* device, const BillboardMeshGeometry::VertexType& instance) const
{
	m_mesh->AddInstance(device, instance);
}
void BillboardRenderItem::AddInstances(ID3D11Device* device, const std::vector<BillboardMeshGeometry::VertexType>& instances) const
{
	m_mesh->AddInstances(device, instances);
}
void BillboardRenderItem::RemoveLastInstance() const
{
	m_mesh->RemoveLastInstance();
}

BillboardMeshGeometry* BillboardRenderItem::GetMesh() const
{
	return m_mesh;
}
void BillboardRenderItem::SetMesh(BillboardMeshGeometry* mesh)
{
	m_mesh = mesh;
}

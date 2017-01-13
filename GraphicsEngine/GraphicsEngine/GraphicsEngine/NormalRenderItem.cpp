#include "stdafx.h"
#include "NormalRenderItem.h"
#include "ImmutableMeshGeometry.h"
#include "SubmeshGeometry.h"

using namespace GraphicsEngine;

void NormalRenderItem::Render(ID3D11DeviceContext* deviceContext) const
{
	SetInputAssemblerData(deviceContext);

	const auto& submesh = GetSubmesh();
	deviceContext->DrawIndexedInstanced(submesh.IndexCount, static_cast<UINT>(m_visibleInstanceCount), submesh.StartIndexLocation, submesh.BaseVertexLocation, 0);
}
void NormalRenderItem::RenderNonInstanced(ID3D11DeviceContext* deviceContext) const
{
	SetInputAssemblerData(deviceContext);

	const auto& submesh = GetSubmesh();
	deviceContext->DrawIndexed(submesh.IndexCount, submesh.StartIndexLocation, submesh.BaseVertexLocation);
}

void NormalRenderItem::AddInstance(const ShaderBufferTypes::InstanceData& instanceData)
{
	//m_colliders.push_back(OctreeCollider(this, static_cast<uint32_t>(this->InstancesData.size())));
	m_instancesData.push_back(instanceData);
}
void NormalRenderItem::SetInstance(size_t instanceID, const ShaderBufferTypes::InstanceData& instanceData)
{
	m_instancesData[instanceID] = instanceData;
}
const ShaderBufferTypes::InstanceData& NormalRenderItem::GetInstance(size_t instanceID)
{
	return m_instancesData[instanceID];
}
void NormalRenderItem::RemoveLastInstance()
{
	if (!m_instancesData.empty())
		m_instancesData.pop_back();
}
void NormalRenderItem::InscreaseInstancesCapacity(size_t aditionalCapacity)
{
	m_instancesData.reserve(m_instancesData.capacity() + aditionalCapacity);
}

void NormalRenderItem::InsertVisibleInstance(size_t instanceID)
{
	m_visibleInstances.insert(static_cast<uint32_t>(instanceID));
}
void NormalRenderItem::ClearVisibleInstances()
{
	m_visibleInstances.clear();
}

ImmutableMeshGeometry* NormalRenderItem::GetMesh() const
{
	return m_mesh;
}

void NormalRenderItem::SetMesh(ImmutableMeshGeometry* mesh, const std::string& submeshName)
{
	m_mesh = mesh;
	m_submeshName = submeshName;
}

const SubmeshGeometry& NormalRenderItem::GetSubmesh() const
{
	return m_mesh->GetSubmesh(m_submeshName);
}

const std::vector<ShaderBufferTypes::InstanceData>& NormalRenderItem::GetInstancesData() const
{
	return m_instancesData;
}
const std::vector<OctreeCollider>& NormalRenderItem::GetColliders() const
{
	return m_colliders;
}
std::vector<OctreeCollider>& NormalRenderItem::GetColliders()
{
	return m_colliders;
}

size_t NormalRenderItem::GetVisibleInstanceCount() const
{
	return m_visibleInstanceCount;
}
void NormalRenderItem::SetVisibleInstanceCount(size_t visibleInstanceCount)
{
	m_visibleInstanceCount = visibleInstanceCount;
}

const std::unordered_set<uint32_t>& NormalRenderItem::GetVisibleInstances() const
{
	return m_visibleInstances;
}

void NormalRenderItem::SetInputAssemblerData(ID3D11DeviceContext* deviceContext) const
{
	std::array<ID3D11Buffer*, 1> vertexBuffers = { m_mesh->GetVertexBuffer() };
	std::array<UINT, 1> strides = { m_mesh->GetStride() };
	std::array<UINT, 1> offsets = { m_mesh->GetOffset() };
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffers.data(), strides.data(), offsets.data());
	deviceContext->IASetIndexBuffer(m_mesh->GetIndexBuffer(), m_mesh->GetIndexFormat(), 0);
	deviceContext->IASetPrimitiveTopology(m_mesh->GetPrimitiveType());
}

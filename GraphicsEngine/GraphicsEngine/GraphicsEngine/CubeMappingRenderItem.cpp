#include "stdafx.h"
#include "CubeMappingRenderItem.h"

#include <DirectXMath.h>

using namespace GraphicsEngine;

CubeMappingRenderItem::CubeMappingRenderItem(ID3D11Device* device, ImmutableMeshGeometry* mesh, const std::string& submeshName, const DirectX::XMFLOAT3& position) :
	m_mesh(mesh),
	m_submeshName(submeshName),
	m_camera(position),
	m_renderTexture(device, 256, 256, DXGI_FORMAT_R8G8B8A8_UNORM),
	m_position(position),
	m_instanceBuffer(device, sizeof(ShaderBufferTypes::InstanceData), sizeof(ShaderBufferTypes::InstanceData))
{
	XMStoreFloat4x4(&m_instanceData.WorldMatrix, DirectX::XMMatrixTranslation(position.x, position.y, position.z));
}

void CubeMappingRenderItem::Render(ID3D11DeviceContext* deviceContext) const
{
	std::array<ID3D11Buffer*, 1> vertexBuffers = { m_mesh->GetVertexBuffer() };
	std::array<UINT, 1> strides = { m_mesh->GetStride() };
	std::array<UINT, 1> offsets = { m_mesh->GetOffset() };
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffers.data(), strides.data(), offsets.data());
	deviceContext->IASetIndexBuffer(m_mesh->GetIndexBuffer(), m_mesh->GetIndexFormat(), 0);
	deviceContext->IASetPrimitiveTopology(m_mesh->GetPrimitiveType());

	UINT stride = sizeof(ShaderBufferTypes::InstanceData);
	UINT offset = 0;
	m_instanceBuffer.CopyData(deviceContext, &m_instanceData, sizeof(ShaderBufferTypes::InstanceData));
	deviceContext->IASetVertexBuffers(1, 1, m_instanceBuffer.GetAddressOf(), &stride, &offset);

	const auto& submesh = m_mesh->GetSubmesh(m_submeshName);
	deviceContext->DrawIndexedInstanced(submesh.IndexCount, 1, submesh.StartIndexLocation, submesh.BaseVertexLocation, 0);
}
void CubeMappingRenderItem::RenderNonInstanced(ID3D11DeviceContext* deviceContext) const
{
	std::array<ID3D11Buffer*, 1> vertexBuffers = { m_mesh->GetVertexBuffer() };
	std::array<UINT, 1> strides = { m_mesh->GetStride() };
	std::array<UINT, 1> offsets = { m_mesh->GetOffset() };
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffers.data(), strides.data(), offsets.data());
	deviceContext->IASetIndexBuffer(m_mesh->GetIndexBuffer(), m_mesh->GetIndexFormat(), 0);
	deviceContext->IASetPrimitiveTopology(m_mesh->GetPrimitiveType());

	const auto& submesh = m_mesh->GetSubmesh(m_submeshName);
	deviceContext->DrawIndexed(submesh.IndexCount, submesh.StartIndexLocation, submesh.BaseVertexLocation);
}

void CubeMappingRenderItem::RemoveLastInstance()
{
}

const CubeMappingCamera& CubeMappingRenderItem::GetCamera() const
{
	return m_camera;
}
const CubeMapRenderTexture& CubeMappingRenderItem::GetRenderTexture() const
{
	return m_renderTexture;
}
const DirectX::XMFLOAT3& CubeMappingRenderItem::GetPosition() const
{
	return m_position;
}
const ShaderBufferTypes::InstanceData& CubeMappingRenderItem::GetInstanceData() const
{
	return m_instanceData;
}

const SubmeshGeometry& CubeMappingRenderItem::GetSubmesh() const
{
	return m_mesh->GetSubmesh(m_submeshName);
}

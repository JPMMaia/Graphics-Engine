#include "stdafx.h"
#include "RenderItem.h"
#include "Graphics.h"

using namespace GraphicsEngine;

RenderItem::RenderItem(SIZE_T maxInstanceCount)
{
	InstancesData.reserve(maxInstanceCount);
}

void RenderItem::Render(ID3D11DeviceContext* deviceContext) const
{
	std::array<ID3D11Buffer*, 1> vertexBuffers = { this->Mesh->GetVertexBuffer() };
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffers.data(), &this->Stride, &this->Offset);
	deviceContext->IASetIndexBuffer(this->Mesh->GetIndexBuffer(), this->IndexFormat, 0);
	deviceContext->IASetPrimitiveTopology(this->PrimitiveType);

	deviceContext->DrawIndexedInstanced(this->IndexCount, static_cast<UINT>(this->VisibleInstanceCount), this->StartIndexLocation, this->BaseVertexLocation, 0);
}
void RenderItem::RenderNonInstanced(ID3D11DeviceContext* deviceContext) const
{
	std::array<ID3D11Buffer*, 1> vertexBuffers = { this->Mesh->GetVertexBuffer() };
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffers.data(), &this->Stride, &this->Offset);
	deviceContext->IASetIndexBuffer(this->Mesh->GetIndexBuffer(), this->IndexFormat, 0);
	deviceContext->IASetPrimitiveTopology(this->PrimitiveType);

	deviceContext->DrawIndexed(this->IndexCount, this->StartIndexLocation, this->BaseVertexLocation);
}

void RenderItem::AddInstance(const ShaderBufferTypes::InstanceData& instanceData)
{
	//this->Colliders.push_back(OctreeCollider(this, static_cast<uint32_t>(this->InstancesData.size())));
	this->InstancesData.push_back(instanceData);
}

void RenderItem::RemoveLastInstance()
{
	if(!this->InstancesData.empty())
		this->InstancesData.pop_back();
}

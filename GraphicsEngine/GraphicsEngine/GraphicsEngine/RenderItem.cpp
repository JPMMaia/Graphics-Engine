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
	deviceContext->IASetVertexBuffers(0, 1, this->Mesh->Vertices.GetAddressOf(), &this->Stride, &this->Offset);
	deviceContext->IASetIndexBuffer(this->Mesh->Indices.Get(), this->IndexFormat, 0);
	deviceContext->IASetPrimitiveTopology(this->PrimitiveType);
	
	deviceContext->DrawIndexedInstanced(this->IndexCount, this->VisibleInstanceCount, this->StartIndexLocation, this->BaseVertexLocation, 0);
}
void RenderItem::RenderNonInstanced(ID3D11DeviceContext* deviceContext) const
{
	deviceContext->IASetVertexBuffers(0, 1, this->Mesh->Vertices.GetAddressOf(), &this->Stride, &this->Offset);
	deviceContext->IASetIndexBuffer(this->Mesh->Indices.Get(), this->IndexFormat, 0);
	deviceContext->IASetPrimitiveTopology(this->PrimitiveType);

	deviceContext->DrawIndexed(this->IndexCount, this->StartIndexLocation, this->BaseVertexLocation);
}

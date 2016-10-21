#include "stdafx.h"
#include "RenderItem.h"
#include "Graphics.h"
#include "VertexTypes.h"

using namespace GraphicsEngine;

RenderItem::RenderItem(SIZE_T maxInstanceCount)
{
	InstancesData.reserve(maxInstanceCount);
}

void RenderItem::Render(ID3D11DeviceContext* deviceContext) const
{
	UINT stride = sizeof(VertexTypes::DefaultVertexType);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, this->Mesh->Vertices.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(this->Mesh->Indices.Get(), DXGI_FORMAT_R16_UINT, 0); // TODO change format
	deviceContext->IASetPrimitiveTopology(this->PrimitiveType);

	deviceContext->DrawIndexedInstanced(this->IndexCount, static_cast<UINT>(this->InstancesData.size()), this->StartIndexLocation, this->BaseVertexLocation, 0);
}

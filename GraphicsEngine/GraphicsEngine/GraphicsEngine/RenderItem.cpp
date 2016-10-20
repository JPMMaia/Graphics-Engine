#include "stdafx.h"
#include "RenderItem.h"
#include "Graphics.h"
#include "VertexTypes.h"

using namespace GraphicsEngine;

void RenderItem::Render(ID3D11DeviceContext* deviceContext) const
{
	// TODO
	UINT stride = sizeof(VertexTypes::DefaultVertexType);
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, this->Mesh->Vertices.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(this->Mesh->Indices.Get(), DXGI_FORMAT_R16_UINT, 0); // TODO change format
	deviceContext->IASetPrimitiveTopology(this->PrimitiveType);

	deviceContext->DrawIndexed(this->IndexCount, this->StartIndexLocation, this->BaseVertexLocation);
}

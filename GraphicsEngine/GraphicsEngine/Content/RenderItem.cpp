#include "stdafx.h"
#include "RenderItem.h"
#include "Graphics.h"

using namespace GraphicsEngine;

RenderItem::RenderItem() :
	FramesDirtyCount(Graphics::GetFrameResourcesCount())
{
}

void RenderItem::Render(ID3D12GraphicsCommandList* commandList) const
{
	// Set vertex buffer:
	auto vertexBufferView = this->Mesh->GetVertexBufferView();
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

	// Set index buffer:
	auto indexBufferView = this->Mesh->GetIndexBufferView();
	commandList->IASetIndexBuffer(&indexBufferView);

	// Set primitive topology:
	commandList->IASetPrimitiveTopology(this->PrimitiveType);

	// Draw mesh:
	commandList->DrawIndexedInstanced(this->IndexCount, 1, this->StartIndexLocation, this->BaseVertexLocation, 0);
}
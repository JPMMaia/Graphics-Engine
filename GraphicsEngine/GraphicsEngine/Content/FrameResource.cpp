#include "stdafx.h"
#include "FrameResource.h"

using namespace GraphicsEngine;

FrameResource::FrameResource(ID3D12Device* d3dDevice, uint32_t passCount, uint32_t objectCount)
{
	// Create command allocator:
	DX::ThrowIfFailed(
		d3dDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(CommandAllocator.GetAddressOf())
			)
		);

	PassConstants = std::make_unique<UploadBuffer<ConstantBufferTypes::PassConstants>>(d3dDevice, passCount, true);
	ObjectConstants = std::make_unique<UploadBuffer<ConstantBufferTypes::ObjectConstants>>(d3dDevice, objectCount, true);
}

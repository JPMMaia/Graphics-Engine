#include "stdafx.h"
#include "FrameResource.h"

using namespace GraphicsEngine;

FrameResource::FrameResource(ID3D12Device* d3dDevice, uint32_t passCount, uint32_t objectCount, uint32_t materialCount)
{
	// Create command allocator:
	DX::ThrowIfFailed(
		d3dDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(CommandAllocator.GetAddressOf())
			)
		);

	PassBuffer = std::make_unique<UploadBuffer<BufferTypes::PassData>>(d3dDevice, passCount, true);
	ObjectsBuffer = std::make_unique<UploadBuffer<BufferTypes::ObjectData>>(d3dDevice, objectCount, true);
	MaterialsBuffer = std::make_unique <UploadBuffer<BufferTypes::MaterialData>>(d3dDevice, materialCount, false);
}

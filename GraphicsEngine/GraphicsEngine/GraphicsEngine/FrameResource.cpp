#include "stdafx.h"
#include "FrameResource.h"

using namespace Common;
using namespace GraphicsEngine;

FrameResource::FrameResource(ID3D12Device* d3dDevice, const std::vector<std::unique_ptr<RenderItem>>& renderItems, uint32_t materialCount, uint32_t passCount) :
	InstancesBufferArray(renderItems.size())
{
	// Create command allocator:
	ThrowIfFailed(
		d3dDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(CommandAllocator.GetAddressOf())
			)
		);

	for(size_t i = 0; i < renderItems.size(); ++i)
		InstancesBufferArray[i] = std::make_unique <UploadBuffer<BufferTypes::InstanceData>>(d3dDevice, renderItems[i]->InstancesData.size(), false);

	MaterialsBuffer = std::make_unique <UploadBuffer<BufferTypes::MaterialData>>(d3dDevice, materialCount, false);
	PassBuffer = std::make_unique<UploadBuffer<BufferTypes::PassData>>(d3dDevice, passCount, true);
}

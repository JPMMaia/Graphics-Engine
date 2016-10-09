#pragma once

#include "BufferTypes.h"
#include "RenderItem.h"
#include "UploadBuffer.h"

#include <memory>

namespace GraphicsEngine
{
	/// <sumary>
	/// Stores the resources needed for the CPU to build the command lists for a frame.
	/// </sumary>
	struct FrameResource
	{
	public:
		FrameResource(ID3D12Device* d3dDevice, const std::vector<std::unique_ptr<RenderItem>>& renderItems, uint32_t materialCount, uint32_t passCount);

	public:
		uint64_t Fence = 0;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocator;

		std::vector<std::unique_ptr<UploadBuffer<BufferTypes::InstanceData>>> InstancesBufferArray;
		std::unique_ptr<UploadBuffer<BufferTypes::MaterialData>> MaterialsBuffer;
		std::unique_ptr<UploadBuffer<BufferTypes::PassData>> PassBuffer;
	};
}

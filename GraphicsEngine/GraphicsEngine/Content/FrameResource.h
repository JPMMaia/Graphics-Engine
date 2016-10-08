#pragma once

#include "UploadBuffer.h"
#include "BufferTypes.h"

#include <memory>

namespace GraphicsEngine
{
	/// <sumary>
	/// Stores the resources needed for the CPU to build the command lists for a frame.
	/// </sumary>
	struct FrameResource
	{
	public:
		FrameResource(ID3D12Device* d3dDevice, uint32_t passCount, uint32_t objectCount, uint32_t materialCount);

	public:
		uint64_t Fence = 0;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocator;

		std::unique_ptr<UploadBuffer<BufferTypes::PassData>> PassBuffer;
		std::unique_ptr<UploadBuffer<BufferTypes::MaterialData>> MaterialsBuffer;
		std::unique_ptr<UploadBuffer<BufferTypes::ObjectData>> ObjectsBuffer;
	};
}

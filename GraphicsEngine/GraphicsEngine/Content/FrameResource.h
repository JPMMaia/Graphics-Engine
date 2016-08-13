#pragma once

#include "UploadBuffer.h"
#include "ConstantBufferTypes.h"

#include <memory>

namespace GraphicsEngine
{
	/// <sumary>
	/// Stores the resources needed for the CPU to build the command lists for a frame.
	/// </sumary>
	struct FrameResource
	{
	public:
		FrameResource(ID3D12Device* d3dDevice, uint32_t passCount, uint32_t objectCount);

	public:
		uint64_t Fence = 0;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocator;

		std::unique_ptr<UploadBuffer<ConstantBufferTypes::PassConstants>> PassConstants;
		std::unique_ptr<UploadBuffer<ConstantBufferTypes::ObjectConstants>> ObjectConstants;
	};
}

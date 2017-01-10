#pragma once

#include "RenderItem.h"
#include "BufferTypes.h"

#include <memory>

namespace GraphicsEngine
{
	struct FrameResource
	{
	public:
		FrameResource() = default;
		FrameResource(ID3D11Device* device, const std::vector<std::unique_ptr<RenderItem>>& renderItems, SIZE_T materialCount);

		void RealocateInstanceBuffer(ID3D11Device* device, RenderItem* renderItem);

	public:
		std::unordered_map<std::string, InstanceBuffer> InstancesBuffers;
		std::vector<DynamicConstantBuffer> MaterialDataArray;
		DynamicConstantBuffer MainPassData;
		DynamicConstantBuffer ShadowPassData;
	};
}

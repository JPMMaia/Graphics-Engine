#pragma once

#include "RenderItem.h"
#include "BufferTypes.h"

#include <memory>
#include <unordered_map>
#include "NormalRenderItem.h"

namespace GraphicsEngine
{
	struct FrameResource
	{
	public:
		FrameResource() = default;
		FrameResource(ID3D11Device* device, const std::vector<NormalRenderItem*>& renderItems, SIZE_T materialCount);

		void RealocateInstanceBuffer(ID3D11Device* device, NormalRenderItem* renderItem);

	public:
		std::unordered_map<std::string, InstanceBuffer> InstancesBuffers;
		std::vector<DynamicConstantBuffer> MaterialDataArray;
		DynamicConstantBuffer MainPassData;
		DynamicConstantBuffer ShadowPassData;
		std::array<DynamicConstantBuffer, 6> CubeMapPassData;
	};
}

#include "stdafx.h"
#include "FrameResource.h"
#include "ShaderBufferTypes.h"

using namespace Common;
using namespace GraphicsEngine;

FrameResource::FrameResource(ID3D11Device* device, const std::vector<std::unique_ptr<RenderItem>>& renderItems, SIZE_T materialCount) :
	MaterialDataArray(materialCount)
{
	// Initialize instances buffers:
	auto stride = static_cast<uint32_t>(sizeof(ShaderBufferTypes::InstanceData));
	for(auto& renderItem : renderItems)
	{
		if(!renderItem->InstancesData.empty())
			InstancesBuffers[renderItem->Name].Initialize(device, static_cast<UINT>(stride * renderItem->InstancesData.size()), stride);
	}
		
	// Initialize material data array:
	constexpr auto materialDataSize = sizeof(ShaderBufferTypes::MaterialData);
	for (SIZE_T i = 0; i < materialCount; ++i)
		MaterialDataArray[i].Initialize(device, materialDataSize, materialDataSize);

	// Initialize pass data buffers:
	constexpr auto passDataSize = sizeof(ShaderBufferTypes::PassData);
	MainPassData.Initialize(device, passDataSize, passDataSize);
	ShadowPassData.Initialize(device, passDataSize, passDataSize);
}

void FrameResource::RealocateInstanceBuffer(ID3D11Device* device, RenderItem* renderItem)
{
	auto& instanceBuffer = InstancesBuffers[renderItem->Name];
	auto bufferStride = static_cast<uint32_t>(sizeof(ShaderBufferTypes::InstanceData));

	auto instanceCount = renderItem->InstancesData.size();

	auto neededBufferSize = instanceCount * bufferStride;
	if(instanceBuffer.GetSize() >= neededBufferSize)
		return;

	// Allocate space for twice as needed:
	auto targetBufferSize = 2 * neededBufferSize;
	instanceBuffer.Initialize(device, targetBufferSize, bufferStride);
}

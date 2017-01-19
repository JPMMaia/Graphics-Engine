#include "stdafx.h"
#include "FrameResource.h"
#include "ShaderBufferTypes.h"
#include "NormalRenderItem.h"

using namespace Common;
using namespace GraphicsEngine;

FrameResource::FrameResource(ID3D11Device* device, const std::vector<NormalRenderItem*>& renderItems, SIZE_T materialCount) :
	MaterialDataArray(materialCount)
{
	// Initialize instances buffers:
	auto stride = static_cast<uint32_t>(sizeof(ShaderBufferTypes::InstanceData));
	for(auto& renderItem : renderItems)
	{
		const auto& instancesData = renderItem->GetInstancesData();
		if(!instancesData.empty())
			InstancesBuffers[renderItem->GetName()].Initialize(device, static_cast<UINT>(stride * instancesData.size()), stride);
	}
		
	// Initialize material data array:
	constexpr auto materialDataSize = sizeof(ShaderBufferTypes::MaterialData);
	for (SIZE_T i = 0; i < materialCount; ++i)
		MaterialDataArray[i].Initialize(device, materialDataSize, materialDataSize);

	// Initialize pass data buffers:
	constexpr auto passDataSize = sizeof(ShaderBufferTypes::PassData);
	MainPassData.Initialize(device, passDataSize, passDataSize);
	ShadowPassData.Initialize(device, passDataSize, passDataSize);

	// Initialize cube map pass data buffers:
	for(size_t i = 0; i < CubeMapPassData.size(); ++i)
	{
		CubeMapPassData[i].Initialize(device, passDataSize, passDataSize);
	}
}

void FrameResource::RealocateInstanceBuffer(ID3D11Device* device, NormalRenderItem* renderItem)
{
	auto& instanceBuffer = InstancesBuffers[renderItem->GetName()];
	auto bufferStride = static_cast<uint32_t>(sizeof(ShaderBufferTypes::InstanceData));

	auto instanceCount = renderItem->GetInstancesData().size();

	auto neededBufferSize = instanceCount * bufferStride;
	if(instanceBuffer.GetSize() >= neededBufferSize)
		return;

	// Allocate space for twice as needed:
	auto targetBufferSize = static_cast<int32_t>(2 * neededBufferSize);
	instanceBuffer.Initialize(device, targetBufferSize, bufferStride);
}

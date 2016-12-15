#include "stdafx.h"
#include "FrameResource.h"
#include "ShaderBufferTypes.h"

using namespace Common;
using namespace GraphicsEngine;

FrameResource::FrameResource(ID3D11Device* device, const std::vector<std::unique_ptr<RenderItem>>& renderItems, SIZE_T materialCount) :
	MaterialDataArray(materialCount)
{
	// Initialize instances buffers:
	for(auto& renderItem : renderItems)
		InstancesBuffers[renderItem->Name].Initialize<ShaderBufferTypes::InstanceData>(device, static_cast<UINT>(sizeof(ShaderBufferTypes::InstanceData) * renderItem->InstancesData.size()));

	// Initialize material data array:
	for (SIZE_T i = 0; i < materialCount; ++i)
		MaterialDataArray[i].Initialize<ShaderBufferTypes::MaterialData>(device, sizeof(ShaderBufferTypes::MaterialData));

	// Initialize pass data buffers:
	MainPassData.Initialize<ShaderBufferTypes::PassData>(device, sizeof(ShaderBufferTypes::PassData));
	ShadowPassData.Initialize<ShaderBufferTypes::PassData>(device, sizeof(ShaderBufferTypes::PassData));
}

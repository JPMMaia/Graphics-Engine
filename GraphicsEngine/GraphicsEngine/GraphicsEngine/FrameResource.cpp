#include "stdafx.h"
#include "FrameResource.h"
#include "ShaderBufferTypes.h"

using namespace Common;
using namespace GraphicsEngine;

FrameResource::FrameResource(ID3D11Device* device, const std::vector<std::unique_ptr<RenderItem>>& renderItems, SIZE_T materialCount) :
	ObjectDataArray(renderItems.size()),
	MaterialDataArray(materialCount)
{
	for(SIZE_T i = 0; i < renderItems.size(); ++i)
		ObjectDataArray[i].Initialize<ShaderBufferTypes::ObjectData>(device, sizeof(ShaderBufferTypes::ObjectData));

	PassData.Initialize<ShaderBufferTypes::PassData>(device, sizeof(ShaderBufferTypes::PassData));

	for (SIZE_T i = 0; i < materialCount; ++i)
		MaterialDataArray[i].Initialize<ShaderBufferTypes::MaterialData>(device, sizeof(ShaderBufferTypes::MaterialData));
}

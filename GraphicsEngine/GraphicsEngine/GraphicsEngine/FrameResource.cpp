#include "stdafx.h"
#include "FrameResource.h"
#include "ShaderBufferTypes.h"

using namespace Common;
using namespace GraphicsEngine;

FrameResource::FrameResource(ID3D11Device* device, const std::vector<std::unique_ptr<RenderItem>>& renderItems) :
	MatrixBufferArray(renderItems.size())
{	
	for(SIZE_T i = 0; i < renderItems.size(); ++i)
		MatrixBufferArray[i].Initialize<ShaderBufferTypes::MatrixBuffer>(device, static_cast<uint32_t>(sizeof(ShaderBufferTypes::MatrixBuffer)));
}

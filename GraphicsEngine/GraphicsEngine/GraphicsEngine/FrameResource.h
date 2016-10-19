#pragma once

#include "RenderItem.h"
#include "ShaderBufferTypes.h"

#include <memory>

namespace GraphicsEngine
{
	struct FrameResource
	{
	public:
		FrameResource() = default;
		FrameResource(ID3D11Device* device, const std::vector<std::unique_ptr<RenderItem>>& renderItems);

	public:
		std::vector<ShaderBufferTypes::MatrixBuffer> MatrixBufferArray;
	};
}

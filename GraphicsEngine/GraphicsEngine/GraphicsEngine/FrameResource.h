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
		FrameResource(ID3D11Device* device, const std::vector<std::unique_ptr<RenderItem>>& renderItems);

	public:
		std::vector<DynamicConstantBuffer> MatrixBufferArray;
	};
}

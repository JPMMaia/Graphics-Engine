﻿#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <cstdint>
#include <DirectXCollision.h>

namespace GraphicsEngine
{
	struct SubmeshGeometry
	{
		uint32_t IndexCount = 0;
		uint32_t StartIndexLocation = 0;
		uint32_t BaseVertexLocation = 0;
		DirectX::BoundingBox Bounds;
	};
}

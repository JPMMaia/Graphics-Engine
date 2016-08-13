#pragma once

#include <DirectXMath.h>
#include "MathHelper.h"
#include "MeshGeometry.h"

namespace GraphicsEngine
{
	struct RenderItem
	{
	public:
		RenderItem();

	public:
		DirectX::XMFLOAT4X4 WorldMatrix = MathHelper::Identity4x4();
		int FramesDirtyCount;
		uint32_t ObjectCBIndex;
		MeshGeometry* Mesh;
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		uint32_t IndexCount = 0;
		uint32_t StartIndexLocation = 0;
		int BaseVertexLocation = 0;
	};
}

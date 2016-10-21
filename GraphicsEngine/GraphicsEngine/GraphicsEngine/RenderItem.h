#pragma once

#include "Common/MathHelper.h"
#include "MeshGeometry.h"
#include "Material.h"

#include <DirectXMath.h>

namespace GraphicsEngine
{
	struct RenderItem
	{
	public:
		void Render(ID3D11DeviceContext* deviceContext) const;

	public:
		MeshGeometry* Mesh = nullptr;
		Material* Material = nullptr;
		D3D_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		uint32_t IndexCount = 0;
		uint32_t StartIndexLocation = 0;
		int BaseVertexLocation = 0;
		DirectX::XMFLOAT4X4 WorldMatrix = MathHelper::Identity4x4();
		int ObjectBufferIndex = 0;
	};
}

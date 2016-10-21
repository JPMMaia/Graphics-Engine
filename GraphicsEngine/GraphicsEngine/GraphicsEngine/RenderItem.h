#pragma once

#include "MeshGeometry.h"
#include "Material.h"
#include "ShaderBufferTypes.h"

#include <DirectXMath.h>
#include <DirectXCollision.h>

namespace GraphicsEngine
{
	struct RenderItem
	{
	public:
		RenderItem() = default;
		explicit RenderItem(SIZE_T maxInstanceCount);

		void Render(ID3D11DeviceContext* deviceContext) const;

	public:
		std::string Name;
		MeshGeometry* Mesh = nullptr;
		Material* Material = nullptr;
		D3D_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		uint32_t IndexCount = 0;
		uint32_t StartIndexLocation = 0;
		int BaseVertexLocation = 0;
		DirectX::BoundingBox Bounds;
		SIZE_T VisibleInstanceCount = 0;

		std::vector<ShaderBufferTypes::InstanceData> InstancesData;
	};
}

#pragma once

#include "MeshGeometry.h"
#include "Material.h"
#include "ShaderBufferTypes.h"
#include "VertexTypes.h"
#include "OctreeCollider.h"

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <unordered_set>

namespace GraphicsEngine
{
	struct RenderItem
	{
	public:
		RenderItem() = default;
		explicit RenderItem(SIZE_T maxInstanceCount);

		void Render(ID3D11DeviceContext* deviceContext) const;
		void RenderNonInstanced(ID3D11DeviceContext* deviceContext) const;

		void AddInstance(const ShaderBufferTypes::InstanceData& instanceData);
		void RemoveLastInstance();

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
		UINT Stride = sizeof(VertexTypes::DefaultVertexType);
		UINT Offset = 0;
		DXGI_FORMAT IndexFormat = DXGI_FORMAT_R32_UINT;

		std::vector<ShaderBufferTypes::InstanceData> InstancesData;
		std::vector<OctreeCollider> Colliders;
		std::unordered_set<uint32_t> VisibleInstances;
	};
}

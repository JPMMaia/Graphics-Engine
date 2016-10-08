#pragma once

#include "MeshGeometry.h"
#include "BufferTypes.h"

#include <DirectXMath.h>

namespace GraphicsEngine
{
	struct RenderItem
	{
	public:
		RenderItem();

		void AddInstance(const BufferTypes::InstanceData& instanceData);
		void Render(ID3D12GraphicsCommandList* commandList) const;

	public:
		int FramesDirtyCount = 0;
		MeshGeometry* Mesh = nullptr;
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		uint32_t IndexCount = 0;
		uint32_t StartIndexLocation = 0;
		int BaseVertexLocation = 0;
		UINT InstanceCount = 0;

		std::vector<BufferTypes::InstanceData> InstancesData;
	};
}

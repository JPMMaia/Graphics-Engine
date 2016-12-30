#pragma once

#include "OctreeBaseCollider.h"

#include <DirectXCollision.h>

namespace GraphicsEngine
{
	struct RenderItem;

	class OctreeCollider : public OctreeBaseCollider
	{
	public:
		OctreeCollider() = default;
		explicit OctreeCollider(RenderItem* renderItem, uint32_t instanceID) :
			m_renderItem(renderItem),
			m_instanceID(instanceID)
		{
		}

		bool Intersects(const DirectX::BoundingBox& box) const override;
		bool XM_CALLCONV Intersects(const DirectX::BoundingFrustum& viewSpaceCameraFrustum, DirectX::FXMMATRIX inverseViewMatrix) const override;

	private:
		RenderItem* m_renderItem;
		uint32_t m_instanceID;
	};
}

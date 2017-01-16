#pragma once

#include <DirectXCollision.h>

namespace GraphicsEngine
{
	class OctreeBaseCollider
	{
	public:
		virtual ~OctreeBaseCollider() = default;

		virtual bool Intersects(const DirectX::BoundingBox& box) const = 0;
		virtual bool XM_CALLCONV Intersects(const DirectX::BoundingFrustum& viewSpaceCameraFrustum, DirectX::FXMMATRIX inverseViewMatrix) const = 0;
	};
}
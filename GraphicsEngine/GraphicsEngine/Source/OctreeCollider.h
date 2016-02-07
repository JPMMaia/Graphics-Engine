#pragma once

#include <DirectXCollision.h>

namespace GraphicsEngine
{
	template<typename Type>
	class OctreeCollider
	{
	public:
		template<
			typename ReturnType = std::result_of_t<decltype(&Type::GetCollider)(Type)>,
			typename = std::enable_if_t<
			std::is_same<ReturnType, const DirectX::BoundingBox&>::value ||
			std::is_same<ReturnType, const DirectX::BoundingSphere&>::value ||
			std::is_same<ReturnType, const DirectX::BoundingOrientedBox&>::value ||
			std::is_same<ReturnType, const DirectX::BoundingFrustum&>::value
			>
		>
			ReturnType GetColliderBase() const
		{
			return static_cast<const Type*>(this)->GetCollider();
		}
	};
}

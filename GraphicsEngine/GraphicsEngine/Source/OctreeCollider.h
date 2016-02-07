#pragma once

#include <DirectXCollision.h>

namespace GraphicsEngine
{
	class OctreeBaseCollider
	{
	protected:
		~OctreeBaseCollider() {}

	public:
		virtual bool Intersects(const DirectX::BoundingBox& box) const = 0;
	};

	template<
		typename Type,
		typename = std::enable_if_t<
		std::is_same<Type, DirectX::BoundingBox>::value ||
		std::is_same<Type, DirectX::BoundingSphere>::value ||
		std::is_same<Type, DirectX::BoundingOrientedBox>::value ||
		std::is_same<Type, DirectX::BoundingFrustum>::value
		>
	>
	class OctreeCollider : public OctreeBaseCollider
	{
	public:
		OctreeCollider() :
			m_collider()
		{
		}
		explicit OctreeCollider(const Type& collider) :
			m_collider(collider)
		{
		}
		explicit OctreeCollider(Type&& collider) :
			m_collider(std::move(collider))
		{
		}
		virtual ~OctreeCollider()
		{
		}

		bool Intersects(const DirectX::BoundingBox& box) const override
		{
			return box.Intersects(m_collider);
		}

	private:
		Type m_collider;
	};
}
#pragma once

#include <DirectXCollision.h>
#include <array>
#include <vector>

#include "OctreeCollider.h"
#include "Common/MemoryPool.h"

namespace GraphicsEngineTester
{
	class OctreeTest;
}

namespace GraphicsEngine
{
	template<
		typename Type,
		size_t MaxObjectsPerLeaf
	>
		class Octree
	{
		friend class GraphicsEngineTester::OctreeTest;

	private:
		struct State
		{
			std::array<Octree<Type, MaxObjectsPerLeaf>*, 8> Children;
			std::vector<Type*> Objects;
		};

	public:
		template<typename = std::enable_if_t<std::is_base_of<OctreeBaseCollider, Type>::value>>
		static Octree<Type, MaxObjectsPerLeaf>& Create(DirectX::BoundingBox&& boundingBox)
		{
			return s_memoryPool.NewElement(std::forward<DirectX::BoundingBox>(boundingBox));
		}

		explicit Octree(DirectX::BoundingBox&& boundingBox) :
			m_boundingBox(boundingBox),
			m_isLeaf(true),
			m_objectCount(0)
		{
		}

		void AddObject(Type* object)
		{
			// Ignore if the object doesn't intersect with bounding box:
			if (!object->Intersects(m_boundingBox))
				return;

			// If not a leaf node:
			if (!m_isLeaf)
			{
				// Add object to child nodes:
				AddObjectToChildNodes(object);
				return;
			}

			// If leaf node has enough space, of if its bounding box is small:
			if (m_objectCount < MaxObjectsPerLeaf || IsSmall())
			{
				AddObjectToArray(object);
				return;
			}

			// If it contains a lot of objects:
			ConvertToNonLeaf(object);
		}
		void FrustumCullObjects(const DirectX::BoundingFrustum& boundingFrustum)
		{
			// Ignore if bounding frustum does not interesect this octree node:
			if (!boundingFrustum.Intersects(m_boundingBox))
				return;

			if (!m_isLeaf)
			{
				// Perform frustum culling on the child nodes:
				for (auto child : m_state.Children)
					child->FrustumCullObjects(boundingFrustum);

				return;
			}

			// For each object inside this octree node:
			for (size_t i = 0; i < m_objectCount; ++i)
			{
				auto object = m_state.Objects[i];

				// If the bounding frustum intersects the object, then we don't cull the object:
				if (object->Intersects(boundingFrustum))
					object->FrustumCull(false);
			}
		}

	private:
		void AddObjectToArray(Type* object)
		{
			auto& objects = m_state.Objects;

			// Try to find object in the array:
			auto location = std::find(objects.cbegin(), objects.cend(), object);

			// If the array contains the object, we can return successfully:
			if (location != objects.end())
				return;

			// Add object to array:
			objects.push_back(object);
			++m_objectCount;
		}
		void AddObjectToChildNodes(Type* object)
		{
			m_objectCount++;

			for (auto& child : m_state.Children)
				child->AddObject(object);
		}
		void ConvertToNonLeaf(Type* newObject)
		{
			using namespace DirectX;

			m_isLeaf = false;

			// Move objects to a temporary list:
			std::vector<Type*> objects(std::move(m_state.Objects));
			m_objectCount -= MaxObjectsPerLeaf;

			// Create child nodes:
			CreateChildNodes();

			// Add objects from temporary list to the child nodes:
			for (auto object : objects)
				AddObjectToChildNodes(object);
			AddObjectToChildNodes(newObject);
		}
		void CreateChildNodes()
		{
			using namespace DirectX;

			// Get corners of bounding box:
			std::array<XMFLOAT3, 8> corners;
			m_boundingBox.GetCorners(corners.data());

			// For each corner:
			auto& children = m_state.Children;
			for (size_t i = 0; i < corners.size(); i++)
			{
				// Create a bounding box, using two points (a corner and the center of the current bounding box):
				BoundingBox boundingBox;
				BoundingBox::CreateFromPoints(boundingBox, XMLoadFloat3(&corners[i]), XMLoadFloat3(&m_boundingBox.Center));

				// Create child octree node:
				children[i] = &Octree::Create(std::move(boundingBox));
			}
		}

		bool IsSmall() const
		{
			auto extents = m_boundingBox.Extents;
			if (extents.x <= 1.0f || extents.y <= 1.0f || extents.z <= 1.0f)
				return true;

			return false;
		}

	private:
		DirectX::BoundingBox m_boundingBox;
		bool m_isLeaf;
		State m_state;
		size_t m_objectCount;

		static MemoryPool<Octree<Type, MaxObjectsPerLeaf>, 100> s_memoryPool;
	};

	template<typename Type, size_t MaxObjectsPerLeaf>
	MemoryPool<Octree<Type, MaxObjectsPerLeaf>, 100> Octree<Type, MaxObjectsPerLeaf>::s_memoryPool;
}

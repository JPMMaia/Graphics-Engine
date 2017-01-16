#pragma once

#include <DirectXCollision.h>
#include <array>
#include <vector>

#include "Common/MemoryPool.h"

namespace GraphicsEngineTester
{
	class OctreeTest;
}

namespace GraphicsEngine
{
	template<
		typename Type
	>
		class Octree
	{
		friend class GraphicsEngineTester::OctreeTest;

	private:
		struct State
		{
			std::array<Octree<Type>*, 8> Children;
			std::vector<Type*> Objects;
		};

	public:
		static constexpr size_t s_memoryPoolSize = 5000;
		static MemoryPool<Octree<Type>, s_memoryPoolSize> s_memoryPool;

	public:
		explicit Octree(size_t objectsPerLeaf, DirectX::BoundingBox&& boundingBox, const DirectX::XMFLOAT3& minExtents) :
			m_boundingBox(boundingBox),
			m_isLeaf(true),
			m_objectCount(0),
			m_objectsPerLeaf(objectsPerLeaf),
			m_minExtents(minExtents)
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

			// If leaf node has enough space, or if its bounding box is small:
			if (m_objectCount < m_objectsPerLeaf || IsSmall())
			{
				AddObjectToArray(object);
				return;
			}

			// If it contains a lot of objects:
			ConvertToNonLeaf(object);
		}
		void XM_CALLCONV CalculateIntersections(const DirectX::BoundingFrustum& viewSpaceBoundingFrustum, DirectX::FXMMATRIX inverseViewMatrix)
		{
			if (!m_isLeaf)
			{
				for (const auto& child : m_state.Children)
					child->CalculateIntersections(viewSpaceBoundingFrustum, inverseViewMatrix);
			}
			else
			{
				for (const auto& object : m_state.Objects)
					object->Intersects(viewSpaceBoundingFrustum, inverseViewMatrix);
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
			m_objectCount -= m_objectsPerLeaf;

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
				children[i] = &Octree::CreateChildOctree(m_objectsPerLeaf, std::move(boundingBox), m_minExtents);
			}
		}
		Octree& CreateChildOctree(size_t objectsPerLeaf, DirectX::BoundingBox&& boundingBox, const DirectX::XMFLOAT3& minExtents)
		{
			return s_memoryPool.NewElement(objectsPerLeaf, std::forward<DirectX::BoundingBox>(boundingBox), minExtents);
		}

		bool IsSmall() const
		{
			auto extents = m_boundingBox.Extents;
			if (extents.x <= m_minExtents.x || extents.y <= m_minExtents.y || extents.z <= m_minExtents.z)
				return true;

			return false;
		}

	private:
		DirectX::BoundingBox m_boundingBox;
		bool m_isLeaf;
		State m_state;
		size_t m_objectCount;
		size_t m_objectsPerLeaf;
		DirectX::XMFLOAT3 m_minExtents;
	};

	template<typename Type>
	MemoryPool<Octree<Type>, Octree<Type>::s_memoryPoolSize> Octree<Type>::s_memoryPool;
}

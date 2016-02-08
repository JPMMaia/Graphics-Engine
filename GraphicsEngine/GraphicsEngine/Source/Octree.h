#pragma once

#include <DirectXCollision.h>

#include "MemoryPool.h"
#include "OctreeCollider.h"

namespace GraphicsEngineTester
{
	class OctreeTest;
}

namespace GraphicsEngine
{
	template<
		typename Type,
		size_t MaxObjectsPerLeaf = 4
	>
	class Octree
	{
		friend class GraphicsEngineTester::OctreeTest;

	private:
		union State
		{
			std::array<Octree<Type>*, 8> Children;
			std::array<Type*, MaxObjectsPerLeaf> Objects;
		};

	public:
		template<typename = std::enable_if_t<std::is_base_of<OctreeBaseCollider, Type>::value>>
		static Octree<Type, MaxObjectsPerLeaf>& Create(DirectX::BoundingBox&& boundingBox);

		explicit Octree(DirectX::BoundingBox&& boundingBox);

		void AddObject(Type* object);



	private:
		bool AddObjectToArray(Type* object);
		void AddObjectToChildNodes(Type* object);
		void ConvertToNonLeaf(Type* newObject);
		void CreateChildNodes();

	private:
		DirectX::BoundingBox m_boundingBox;
		bool m_isLeaf;
		State m_state;
		size_t m_objectCount;

		static MemoryPool<Octree<Type, MaxObjectsPerLeaf>, 100> s_memoryPool;
	};

	template<typename Type, size_t MaxObjectsPerLeaf>
	MemoryPool<Octree<Type, MaxObjectsPerLeaf>, 100> Octree<Type, MaxObjectsPerLeaf>::s_memoryPool;

	template <typename Type, size_t MaxObjectsPerLeaf>
	template <typename>
	Octree<Type, MaxObjectsPerLeaf>& Octree<Type, MaxObjectsPerLeaf>::Create(DirectX::BoundingBox&& boundingBox)
	{		
		return s_memoryPool.NewElement(std::forward<DirectX::BoundingBox>(boundingBox));
	}

	template <typename Type, size_t MaxObjectsPerLeaf>
	Octree<Type, MaxObjectsPerLeaf>::Octree(DirectX::BoundingBox&& boundingBox):
		m_boundingBox(boundingBox),
		m_isLeaf(true),
		m_state(),
		m_objectCount(0)
	{
	}

	template <typename Type, size_t MaxObjectsPerLeaf>
	void Octree<Type, MaxObjectsPerLeaf>::AddObject(Type* object)
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
			
		// Add object to array. If it is full, then convert node to a non-leaf node:
		if (!AddObjectToArray(object))
			ConvertToNonLeaf(object);
	}

	template <typename Type, size_t MaxObjectsPerLeaf>
	bool Octree<Type, MaxObjectsPerLeaf>::AddObjectToArray(Type* object)
	{
		auto& objects = m_state.Objects;

		// Try to find object in the array:
		auto location = std::find(objects.cbegin(), objects.cend(), object);

		// If the array contains the object, we can return successfully:
		if (location != objects.end())
			return true;

		// If the array is full:
		if (m_objectCount >= objects.size())
			return false;

		// Add object to array:
		objects[m_objectCount++] = object;

		return true;
	}

	template <typename Type, size_t MaxObjectsPerLeaf>
	void Octree<Type, MaxObjectsPerLeaf>::AddObjectToChildNodes(Type* object)
	{
		m_objectCount++;

		auto& children = m_state.Children;
		for (auto child = children.cbegin(); child != children.cend(); ++child)
			(*child)->AddObject(object);
	}

	template <typename Type, size_t MaxObjectsPerLeaf>
	void Octree<Type, MaxObjectsPerLeaf>::ConvertToNonLeaf(Type* newObject)
	{
		using namespace DirectX;

		// Convert only if current bounding box extents are greater than a minimun value:
		auto extents = m_boundingBox.Extents;
		if (extents.x <= 1.0f || extents.y <= 1.0f || extents.z <= 1.0f)
			return;

		m_isLeaf = false;

		// Move objects to a temporary list:
		auto objects = std::array<Type*, MaxObjectsPerLeaf>(std::move(m_state.Objects));
		m_objectCount -= MaxObjectsPerLeaf;

		// Create child nodes:
		CreateChildNodes();

		// Add objects from temporary list to the child nodes:
		for (auto object = objects.cbegin(); object != objects.cend(); ++object)
			AddObjectToChildNodes(*object);
		AddObjectToChildNodes(newObject);
	}

	template <typename Type, size_t MaxObjectsPerLeaf>
	void Octree<Type, MaxObjectsPerLeaf>::CreateChildNodes()
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
}

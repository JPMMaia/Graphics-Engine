#pragma once

#include <DirectXCollision.h>

#include "MemoryPool.h"

namespace GraphicsEngineTester
{
	class OctreeTest;
}

namespace GraphicsEngine
{
	template<typename Type, size_t MaxObjectsPerLeaf = 4>
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
		static Octree<Type, MaxObjectsPerLeaf>& Create(DirectX::BoundingBox&& boundingBox)
		{
			return s_memoryPool.NewElement(std::forward<DirectX::BoundingBox>(boundingBox));
		}

		explicit Octree(DirectX::BoundingBox&& boundingBox) :
			m_boundingBox(boundingBox),
			m_isLeaf(true),
			m_state(),
			m_objectCount(0)
		{
		}

		void AddObject(Type* object)
		{
			// If is leaf node:
			if(m_isLeaf)
			{
				// Add object to array. If it is full, then convert node to a non-leaf node:
				if (!AddObjectToArray(object))
					ConvertToNonLeaf();
			}
		}

	private:
		bool AddObjectToArray(Type* object)
		{
			auto& objects = m_state.Objects;

			// Try to find object in the array:
			auto location = std::find(objects.cbegin(), objects.cend(), object);

			// If the array contains the object, we can return successfully:
			if (location != objects.end())
				return true;

			// If the array is full:
			if (m_objectCount == MaxObjectsPerLeaf)
				return false;

			// Add object to array:
			objects[m_objectCount++] = object;			

			return true;
		}

		void ConvertToNonLeaf()
		{
			m_isLeaf = false;


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

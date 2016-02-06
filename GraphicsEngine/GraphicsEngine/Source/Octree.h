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
		explicit Octree(DirectX::BoundingBox&& boundingBox) :
			m_boundingBox(boundingBox),
			m_isLeaf(true),
			m_state(),
			m_objectCounter(0)
		{
		}

		void AddObject(Type* object)
		{
			// If is leaf node:
			if(m_isLeaf)
			{
				// If max number of objects in the leaf not yet reached:
				if (m_objectCounter != MaxObjectsPerLeaf)
					m_state.Objects[m_objectCounter++] = object;

				// Else, this node needs to be converted to a non leaf node:
				else
					ConvertToNonLeaf();
			}
		}

	private:
		void ConvertToNonLeaf()
		{
			m_isLeaf = false;


		}

	private:
		static MemoryPool<Octree, 100> s_memoryPool;

		DirectX::BoundingBox m_boundingBox;
		bool m_isLeaf;
		State m_state;
		size_t m_objectCounter;
	};
}

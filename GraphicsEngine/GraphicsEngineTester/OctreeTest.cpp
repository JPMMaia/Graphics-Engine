#include "stdafx.h"
#include "CppUnitTest.h"

#include <Octree.h>

using namespace DirectX;
using namespace GraphicsEngine;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace GraphicsEngineTester
{
	class ColliderTestClass : private OctreeCollider<ColliderTestClass>
	{
	public:
		ColliderTestClass() :
			m_boundingBox(),
			m_id(s_counter++)
		{
		}
		~ColliderTestClass()
		{
		}

		const BoundingBox& GetCollider() const
		{
			return m_boundingBox;
		}

	private:
		BoundingBox m_boundingBox;
		size_t m_id;
		static size_t s_counter;
	};
	size_t ColliderTestClass::s_counter = 0;

	TEST_CLASS(OctreeTest)
	{
	public:
		TEST_METHOD(TestOctree)
		{	
			array<ColliderTestClass, 10> gameObjects;

			auto octree = Octree<ColliderTestClass, 4>::Create(
				BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(8.0f, 8.0f, 8.0f))
				);
			Assert::IsTrue(octree.m_isLeaf);

			octree.AddObject(&gameObjects[0]);
			octree.AddObject(&gameObjects[1]);
			octree.AddObject(&gameObjects[2]);
			octree.AddObject(&gameObjects[3]);
			Assert::IsTrue(octree.m_isLeaf);

			// Do not allow to add duplicate objects:
			octree.AddObject(&gameObjects[1]);
			Assert::IsTrue(octree.m_isLeaf);

			octree.AddObject(&gameObjects[4]);
			Assert::IsFalse(octree.m_isLeaf);

			auto& children = octree.m_state.Children;
			for (auto child = children.begin(); child != children.end(); ++child)
				Assert::IsNotNull(*child);

			octree.AddObject(&gameObjects[5]);
			octree.AddObject(&gameObjects[6]);
			octree.AddObject(&gameObjects[7]);
			octree.AddObject(&gameObjects[8]);
			octree.AddObject(&gameObjects[9]);
			Assert::AreEqual(10U, octree.m_objectCount);
		}
	};
}
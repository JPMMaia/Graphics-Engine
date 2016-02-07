#include "stdafx.h"
#include "CppUnitTest.h"

#include <Octree.h>

using namespace DirectX;
using namespace GraphicsEngine;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace GraphicsEngineTester
{
	class GameObjectTestClass
	{
	public:
		GameObjectTestClass() :
			m_id(s_counter++)
		{
		}
		~GameObjectTestClass()
		{
		}

	private:
		size_t m_id;
		static size_t s_counter;
	};
	size_t GameObjectTestClass::s_counter = 0;

	TEST_CLASS(OctreeTest)
	{
	public:
		TEST_METHOD(TestOctree)
		{
			array<GameObjectTestClass, 10> gameObjects;

			auto octree = Octree<GameObjectTestClass, 4>::Create(
				BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(50.0f, 50.0f, 50.0f))
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
		}
	};
}
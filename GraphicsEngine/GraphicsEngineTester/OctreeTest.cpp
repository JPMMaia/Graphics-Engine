#include "stdafx.h"
#include "CppUnitTest.h"

#include <Camera.h>
#include <Octree.h>

using namespace DirectX;
using namespace GraphicsEngine;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace GraphicsEngineTester
{
	bool operator==(const XMFLOAT3& v1, const XMFLOAT3& v2)
	{
		return
			(v1.x == v2.x) &&
			(v1.y == v2.y) &&
			(v1.z == v2.z);
	}

	class ColliderTestClass : public OctreeCollider<BoundingBox>
	{
	public:
		ColliderTestClass() :
			OctreeCollider(),
			m_id(0)
		{
		}
		explicit ColliderTestClass(BoundingBox&& box) :
			OctreeCollider(forward<BoundingBox>(box)),
			m_id(s_counter++)
		{
		}
		~ColliderTestClass()
		{
		}

	private:
		size_t m_id;
		static size_t s_counter;
	};
	size_t ColliderTestClass::s_counter = 0;

	TEST_CLASS(OctreeTest)
	{
	private:
		array<ColliderTestClass, 11> m_gameObjects;

	public:
		TEST_METHOD_INITIALIZE(TestInitialize)
		{
			m_gameObjects =
			{
				ColliderTestClass({ { -6.5f, 0.0f, 6.5f },{ 0.5f, 0.5f, 0.5f } }),
				ColliderTestClass({ { -3.5f, 0.0f, 4.5f },{ 0.5f, 0.5f, 0.5f } }),
				ColliderTestClass({ { -4.5f, 0.0f, 7.5f },{ 0.5f, 0.5f, 0.5f } }),
				ColliderTestClass({ { -4.5f, 0.0f, 10.5f },{ 0.5f, 0.5f, 0.5f } }),
				ColliderTestClass({ { -2.5f, 0.0f, -1.5f },{ 0.5f, 0.5f, 0.5f } }),
				ColliderTestClass({ { 3.5f, 0.0f, -2.5f },{ 0.5f, 0.5f, 0.5f } }),
				ColliderTestClass({ { -6.5f, 0.0f, -6.5f },{ 0.5f, 0.5f, 0.5f } }),
				ColliderTestClass({ { -2.5f, 0.0f, 2.5f },{ 0.5f, 0.5f, 0.5f } }),
				ColliderTestClass({ { -7.5f, 0.0f, 4.5f },{ 0.5f, 0.5f, 0.5f } }),
				ColliderTestClass({ { -5.5f, 0.0f, 2.5f },{ 0.5f, 0.5f, 0.5f } }),
				ColliderTestClass({ { 0.0f, 6.0f, 0.0f },{ 1.0f, 1.0f, 1.0f } }),
			};
		}

		TEST_METHOD(TestOctreeConstruction)
		{
			auto octree = Octree<OctreeBaseCollider, 4>::Create(
				BoundingBox(XMFLOAT3(0.0f, 6.0f, 0.0f), XMFLOAT3(8.0f, 8.0f, 8.0f))
				);
			Assert::IsTrue(octree.m_isLeaf);

			// Add objects to the octree
			octree.AddObject(&m_gameObjects[0]);
			octree.AddObject(&m_gameObjects[1]);
			octree.AddObject(&m_gameObjects[2]);
			Assert::IsTrue(octree.m_isLeaf);
			Assert::AreEqual(3U, octree.m_objectCount);

			// Do not allow to add duplicate objects:
			octree.AddObject(&m_gameObjects[1]);
			Assert::AreEqual(3U, octree.m_objectCount);

			// Add an object which lies outside of the bounding box (it should be ignored):
			octree.AddObject(&m_gameObjects[3]);
			Assert::AreEqual(3U, octree.m_objectCount);

			// Force octree to build new child nodes:
			octree.AddObject(&m_gameObjects[4]);
			octree.AddObject(&m_gameObjects[5]);
			Assert::IsFalse(octree.m_isLeaf);
			Assert::AreEqual(5U, octree.m_objectCount);

			// Test if child nodes were successfully initialized:
			const static XMFLOAT3 EXPECTED_CHILDREN_EXTENTS = { 4.0f, 4.0f, 4.0f };
			auto& children = octree.m_state.Children;
			for (auto child = children.begin(); child != children.end(); ++child)
			{
				Assert::IsNotNull(*child);
				Assert::IsTrue(EXPECTED_CHILDREN_EXTENTS == (*child)->m_boundingBox.Extents);
			}

			// Check if the objects were correctly inserted into the child nodes:
			Assert::AreEqual(3U, children[0]->m_objectCount);
			Assert::AreEqual(0U, children[1]->m_objectCount);
			Assert::AreEqual(1U, children[4]->m_objectCount);
			Assert::AreEqual(1U, children[5]->m_objectCount);

			// Add some more objects:
			octree.AddObject(&m_gameObjects[6]);
			octree.AddObject(&m_gameObjects[7]);
			octree.AddObject(&m_gameObjects[8]);
			octree.AddObject(&m_gameObjects[9]);

			// Check if objects were correctly inserted:
			Assert::AreEqual(9U, octree.m_objectCount);
			Assert::AreEqual(6U, children[0]->m_objectCount);
			Assert::AreEqual(0U, children[1]->m_objectCount);
			Assert::AreEqual(2U, children[4]->m_objectCount);
			Assert::AreEqual(1U, children[5]->m_objectCount);

			// Insert an object which will be shared by multiple nodes:
			octree.AddObject(&m_gameObjects[10]);
			Assert::AreEqual(10U, octree.m_objectCount);
			Assert::AreEqual(7U, children[0]->m_objectCount);
			Assert::AreEqual(1U, children[1]->m_objectCount);
			Assert::AreEqual(1U, children[2]->m_objectCount);
			Assert::AreEqual(1U, children[3]->m_objectCount);
			Assert::AreEqual(3U, children[4]->m_objectCount);
			Assert::AreEqual(2U, children[5]->m_objectCount);
			Assert::AreEqual(1U, children[6]->m_objectCount);
			Assert::AreEqual(1U, children[7]->m_objectCount);
		}

		TEST_METHOD(TestOctreeFrustumIntersection)
		{
			auto camera = Camera();
			auto cameraFrustum = camera.BuildWorldSpaceBoundingFrustum();

			auto octree = Octree<OctreeBaseCollider, 4>::Create(
				BoundingBox(XMFLOAT3(0.0f, 6.0f, 0.0f), XMFLOAT3(8.0f, 8.0f, 8.0f))
				);
			for (auto& object : m_gameObjects)
				octree.AddObject(&object);

			// Set all objects to be culled:
			for (auto& object : m_gameObjects)
				object.FrustumCull(true);

			octree.FrustumCullObjects(cameraFrustum);

			Assert::IsFalse(m_gameObjects[0].IsFrustumCulled());
			Assert::IsFalse(m_gameObjects[1].IsFrustumCulled());
			Assert::IsFalse(m_gameObjects[2].IsFrustumCulled());
			Assert::IsFalse(m_gameObjects[3].IsFrustumCulled());
			Assert::IsTrue(m_gameObjects[4].IsFrustumCulled());
			Assert::IsTrue(m_gameObjects[5].IsFrustumCulled());
			Assert::IsTrue(m_gameObjects[6].IsFrustumCulled());
			Assert::IsFalse(m_gameObjects[7].IsFrustumCulled());
			Assert::IsTrue(m_gameObjects[8].IsFrustumCulled());
			Assert::IsTrue(m_gameObjects[9].IsFrustumCulled());
			Assert::IsTrue(m_gameObjects[10].IsFrustumCulled());
		}
	};
}
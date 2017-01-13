#include "stdafx.h"
#include "CppUnitTest.h"

#include "GraphicsEngine/Octree.h"
#include "GraphicsEngine/Camera.h"
#include "GraphicsEngine/OctreeCollider.h"
#include "GraphicsEngine/RenderItem.h"

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

	class ColliderTestClass : public OctreeBaseCollider
	{
	public:
		ColliderTestClass() :
			m_id(0)
		{
		}
		explicit ColliderTestClass(BoundingBox&& box) :
			m_id(s_counter++),
			m_box(std::move(box))
		{
		}

		bool Intersects(const DirectX::BoundingBox& box) const override
		{
			return box.Intersects(m_box);
		}
		bool __vectorcall Intersects(const DirectX::BoundingFrustum& viewSpaceCameraFrustum, DirectX::FXMMATRIX inverseViewMatrix) const override
		{
			throw runtime_error("Not Implemented");
		}

	private:
		size_t m_id;
		BoundingBox m_box;
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
			auto octree = Octree<OctreeBaseCollider>(
				4,
				BoundingBox(XMFLOAT3(0.0f, 6.0f, 0.0f), XMFLOAT3(8.0f, 8.0f, 8.0f)),
				XMFLOAT3(1.0f, 1.0f, 1.0f)
				);
			Assert::IsTrue(octree.m_isLeaf);

			// Add objects to the octree:
			octree.AddObject(&m_gameObjects[0]);
			octree.AddObject(&m_gameObjects[1]);
			octree.AddObject(&m_gameObjects[2]);
			Assert::IsTrue(octree.m_isLeaf);
			Assert::AreEqual(static_cast<size_t>(3), octree.m_objectCount);

			// Do not allow to add duplicate objects:
			octree.AddObject(&m_gameObjects[1]);
			Assert::AreEqual(static_cast<size_t>(3), octree.m_objectCount);

			// Add an object which lies outside of the bounding box (it should be ignored):
			octree.AddObject(&m_gameObjects[3]);
			Assert::AreEqual(static_cast<size_t>(3), octree.m_objectCount);

			// Force octree to build new child nodes:
			octree.AddObject(&m_gameObjects[4]);
			octree.AddObject(&m_gameObjects[5]);
			Assert::IsFalse(octree.m_isLeaf);
			Assert::AreEqual(static_cast<size_t>(5), octree.m_objectCount);

			// Test if child nodes were successfully initialized:
			const static XMFLOAT3 EXPECTED_CHILDREN_EXTENTS = { 4.0f, 4.0f, 4.0f };
			auto& children = octree.m_state.Children;
			for (auto child = children.begin(); child != children.end(); ++child)
			{
				Assert::IsNotNull(*child);
				Assert::IsTrue(EXPECTED_CHILDREN_EXTENTS == (*child)->m_boundingBox.Extents);
			}

			// Check if the objects were correctly inserted into the child nodes:
			Assert::AreEqual(static_cast<size_t>(3), children[0]->m_objectCount);
			Assert::AreEqual(static_cast<size_t>(0), children[1]->m_objectCount);
			Assert::AreEqual(static_cast<size_t>(1), children[4]->m_objectCount);
			Assert::AreEqual(static_cast<size_t>(1), children[5]->m_objectCount);

			// Add some more objects:
			octree.AddObject(&m_gameObjects[6]);
			octree.AddObject(&m_gameObjects[7]);
			octree.AddObject(&m_gameObjects[8]);
			octree.AddObject(&m_gameObjects[9]);

			// Check if objects were correctly inserted:
			Assert::AreEqual(static_cast<size_t>(9), octree.m_objectCount);
			Assert::AreEqual(static_cast<size_t>(6), children[0]->m_objectCount);
			Assert::AreEqual(static_cast<size_t>(0), children[1]->m_objectCount);
			Assert::AreEqual(static_cast<size_t>(2), children[4]->m_objectCount);
			Assert::AreEqual(static_cast<size_t>(1), children[5]->m_objectCount);

			// Insert an object which will be shared by multiple nodes:
			octree.AddObject(&m_gameObjects[10]);
			Assert::AreEqual(static_cast<size_t>(10), octree.m_objectCount);
			Assert::AreEqual(static_cast<size_t>(7), children[0]->m_objectCount);
			Assert::AreEqual(static_cast<size_t>(1), children[1]->m_objectCount);
			Assert::AreEqual(static_cast<size_t>(1), children[2]->m_objectCount);
			Assert::AreEqual(static_cast<size_t>(1), children[3]->m_objectCount);
			Assert::AreEqual(static_cast<size_t>(3), children[4]->m_objectCount);
			Assert::AreEqual(static_cast<size_t>(2), children[5]->m_objectCount);
			Assert::AreEqual(static_cast<size_t>(1), children[6]->m_objectCount);
			Assert::AreEqual(static_cast<size_t>(1), children[7]->m_objectCount);
		}

		TEST_METHOD(TestOctreeFrustumIntersection)
		{
			Camera camera(16.0f / 9.0f, 90.0f, 0.01f, 1000.0f, XMMatrixIdentity());
			
			// Get view matrix and calculate its inverse:
			camera.Update();
			auto viewMatrix = camera.GetViewMatrix();
			auto viewMatrixDeterminant = XMMatrixDeterminant(viewMatrix);
			auto inverseViewMatrix = XMMatrixInverse(&viewMatrixDeterminant, viewMatrix);
			auto cameraFrustum = camera.BuildViewSpaceBoundingFrustum();

			auto octree = Octree<OctreeBaseCollider>(
				4,
				BoundingBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(8.0f, 8.0f, 8.0f)),
				XMFLOAT3(1.0f, 1.0f, 1.0f)
				);

			RenderItem renderItem;
			renderItem.Bounds = { { 0.0f, 0.0f, 0.0f },{ 0.5f, 0.5f, 0.5f } };
			{
				ShaderBufferTypes::InstanceData data;

				XMStoreFloat4x4(&data.WorldMatrix, XMMatrixTranslation(0.0f, 0.0f, 5.0f));
				renderItem.AddInstance(data);

				XMStoreFloat4x4(&data.WorldMatrix, XMMatrixTranslation(0.0f, 0.0f, -5.0f));
				renderItem.AddInstance(data);
			}

			for (auto& object : renderItem.m_colliders)
				octree.AddObject(&object);

			octree.CalculateIntersections(cameraFrustum, inverseViewMatrix);
			Assert::AreEqual(static_cast<size_t>(1), renderItem.m_visibleInstances.size());
			Assert::AreEqual(0U, *renderItem.m_visibleInstances.begin());
		}
	};
}
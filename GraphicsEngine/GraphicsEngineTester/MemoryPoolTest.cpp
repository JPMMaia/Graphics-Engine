#include "stdafx.h"
#include "CppUnitTest.h"

#include "Common/MemoryPool.h"

using namespace GraphicsEngine;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace GraphicsEngineTester
{
	class MemoryPoolTestClass
	{
	public:
		MemoryPoolTestClass() = default;
		explicit MemoryPoolTestClass(size_t id) : 
			m_id(id)
		{
		}

		size_t GetID() const
		{
			return m_id;
		}

	private:
		size_t m_id = -1;
	};

	TEST_CLASS(MemoryPoolTest)
	{
	public:
		TEST_METHOD(TestMemoryPool)
		{
			// Create a memory pool:
			MemoryPool<MemoryPoolTestClass, 4> memoryPool;

			// Create a first element:
			auto& element1 = memoryPool.NewElement(1);
			Assert::IsTrue(&element1 == &memoryPool[0]);
			Assert::AreEqual(static_cast<size_t>(1), element1.GetID());
			
			// Create a second element:
			auto& element2 = memoryPool.NewElement(2);
			Assert::IsTrue(&element2 == &memoryPool[1]);
			Assert::AreEqual(static_cast<size_t>(2), element2.GetID());

			// Create some more elements:
			memoryPool.NewElement(3);
			memoryPool.NewElement(4);

			// Assert that the active elements counter is correct:
			Assert::AreEqual(static_cast<size_t>(4), memoryPool.GetActiveElements());

			// Delete an element:
			memoryPool.DeleteElement(element2);
			Assert::AreEqual(static_cast<size_t>(3), memoryPool.GetActiveElements());

			// Create another element:
			auto& element5 = memoryPool.NewElement(5);
			Assert::AreEqual(static_cast<size_t>(4), memoryPool.GetActiveElements());
			Assert::IsTrue(&element5 == &memoryPool[1]);
			Assert::AreEqual(static_cast<size_t>(5), element5.GetID());

			// Delete an element twice:
			memoryPool.DeleteElement(element5);
			memoryPool.DeleteElement(element5);
			Assert::AreEqual(static_cast<size_t>(3), memoryPool.GetActiveElements());
		}
	};
}
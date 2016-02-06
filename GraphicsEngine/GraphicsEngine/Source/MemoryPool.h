#pragma once

#include <array>

#include "MemoryPoolElement.h"

namespace GraphicsEngine
{
	template<typename Type, size_t Size>
	class MemoryPool
	{
	public:
		MemoryPool();

		template<typename... ArgumentsType>
		Type& NewElement(ArgumentsType&&... arguments);

		void DeleteElement(Type& element);

		Type& operator[](size_t index);

		size_t GetActiveElements() const;
		constexpr size_t GetMaxSize() const;

	private:
		std::array<MemoryPoolElement<Type>, Size> m_pool;
		size_t m_activeElements;

		MemoryPoolElement<Type>* m_firstAvailable;
	};

	template <typename Type, size_t Size>
	MemoryPool<Type, Size>::MemoryPool():
		m_activeElements(0),
		m_firstAvailable(&m_pool[0])
	{
		// Initialize a list of available elements:
		for (size_t i = 0; i < m_pool.size() - 1; i++)
			m_pool[i].SetNext(&m_pool[i + 1]);

		// Set the last element to end with a nullptr:
		m_pool[Size - 1].SetNext(nullptr);
	}

	template <typename Type, size_t Size>
	template <typename ... ArgumentsType>
	Type& MemoryPool<Type, Size>::NewElement(ArgumentsType&&... arguments)
	{
		// Make sure that the pool isn't full:
		assert(m_firstAvailable != nullptr);

		++m_activeElements;

		// Remove new element from the list of available elements:
		auto& newElement = *m_firstAvailable;
		m_firstAvailable = newElement.GetNext();

		// Initialize element:
		newElement.Initialize(std::forward<ArgumentsType>(arguments)...);

		// Return pointer to element:
		return newElement.GetElement();
	}

	template <typename Type, size_t Size>
	void MemoryPool<Type, Size>::DeleteElement(Type& element)
	{
		// Calculating the index of the element:
		auto baseAddress = reinterpret_cast<size_t>(&m_pool[0]);
		auto elementAddress = reinterpret_cast<size_t>(&element);
		auto index = (elementAddress - baseAddress) / sizeof(MemoryPoolElement<Type>);

		auto& poolElement = m_pool[index];
		if(poolElement.IsInitialized())
		{
			poolElement.Shutdown(m_firstAvailable);
			m_firstAvailable = &poolElement;

			--m_activeElements;
		}
	}

	template <typename Type, size_t Size>
	Type& MemoryPool<Type, Size>::operator[](size_t index)
	{
		return m_pool[index].GetElement();
	}

	template <typename Type, size_t Size>
	size_t MemoryPool<Type, Size>::GetActiveElements() const
	{
		return m_activeElements;
	}
	template <typename Type, size_t Size>
	constexpr size_t MemoryPool<Type, Size>::GetMaxSize() const
	{
		return Size;
	}
}

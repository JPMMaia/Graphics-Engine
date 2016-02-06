#pragma once

namespace GraphicsEngine
{
	template<typename Type>
	class MemoryPoolElement
	{
	private:
		union State
		{
			Type element;
			MemoryPoolElement* next;

			State()
			{
			}
			~State()
			{
			}
		};

	public:
		MemoryPoolElement();

		template<typename... ArgumentsType>
		void Initialize(ArgumentsType&&... arguments);

		void Shutdown(MemoryPoolElement* next);

		MemoryPoolElement* GetNext() const;
		Type& GetElement();

		void SetNext(MemoryPoolElement* next);

		bool IsInitialized() const;

	private:
		State m_state;
		bool m_initialized;
	};

	template <typename Type>
	MemoryPoolElement<Type>::MemoryPoolElement():
		m_state(),
		m_initialized(false)
	{
		m_state.next = nullptr;
	}

	template <typename Type>
	template <typename ... ArgumentsType>
	void MemoryPoolElement<Type>::Initialize(ArgumentsType&&... arguments)
	{
		m_initialized = true;
		m_state.element = Type(std::forward<ArgumentsType>(arguments)...);
	}

	template <typename Type>
	void MemoryPoolElement<Type>::Shutdown(MemoryPoolElement* next)
	{
		m_state.element.~Type();
		m_state.next = next;
		m_initialized = false;
	}

	template <typename Type>
	MemoryPoolElement<Type>* MemoryPoolElement<Type>::GetNext() const
	{
		return m_state.next;
	}
	template <typename Type>
	Type& MemoryPoolElement<Type>::GetElement()
	{
		return m_state.element;
	}

	template <typename Type>
	void MemoryPoolElement<Type>::SetNext(MemoryPoolElement* next)
	{
		m_state.next = next;
	}

	template <typename Type>
	bool MemoryPoolElement<Type>::IsInitialized() const
	{
		return m_initialized;
	}
}
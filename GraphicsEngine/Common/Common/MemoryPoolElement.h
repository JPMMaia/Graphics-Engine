#pragma once

namespace GraphicsEngine
{
	template<typename Type>
	class MemoryPoolElement
	{
	private:
		union State
		{
			Type Element;
			MemoryPoolElement* Next = nullptr;

			template<typename... ArgumentsType>
			explicit State(ArgumentsType&&... arguments) :
				Element(std::forward<ArgumentsType>(arguments)...)
			{
			}
			~State()
			{
			}
		};

	public:
		MemoryPoolElement() = default;
		~MemoryPoolElement() = default;
		
		template<typename... ArgumentsType>
		explicit MemoryPoolElement(ArgumentsType&&... arguments) :
			m_state(std::forward<ArgumentsType>(arguments)...),
			m_initialized(true)
		{
		}

		template<typename... ArgumentsType>
		void Initialize(ArgumentsType&&... arguments)
		{
			m_state.Element = Type(std::forward<ArgumentsType>(arguments)...);
			m_initialized = true;
		}

		void Shutdown(MemoryPoolElement* next)
		{
			m_state.Element.~Type();
			m_state.Next = next;
			m_initialized = false;
		}

		MemoryPoolElement* GetNext() const
		{
			return m_state.Next;
		}
		Type& GetElement()
		{
			return m_state.Element;
		}
		const Type& GetElement() const
		{
			return m_state.Element;
		}

		void SetNext(MemoryPoolElement* next)
		{
			m_state.Next = next;
		}

		bool IsInitialized() const
		{
			return m_initialized;
		}

	private:
		State m_state;
		bool m_initialized = false;
	};
}
#pragma once
#include <vector>
#include <functional>

namespace Common
{
	template<typename EventArgsType>
	class Event
	{
	public:
		using EventFunctionType = std::function<void(const void*, const EventArgsType&)>;

	public:
		void Subscribe(const EventFunctionType& onRaiseEvent)
		{
			m_subscribers.push_back(onRaiseEvent);
		}

		void Raise(const void* sender, const EventArgsType& eventArgs) const
		{
			for(const auto& subscriberFunction : m_subscribers)
			{
				subscriberFunction(sender, eventArgs);
			}
		}

	private:
		std::vector<EventFunctionType> m_subscribers;
	};
}

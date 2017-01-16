#pragma once

#include <chrono>

namespace Common
{
	class PerformanceTimer
	{
	private:
		using TimePointType = std::chrono::time_point<std::chrono::high_resolution_clock>;

	public:
		void Start();
		void End();

		template <class Representation, class Period>
		std::chrono::duration<Representation, Period> ElapsedTime() const;

	private:
		TimePointType m_start;
		TimePointType m_end;
	};

	template <typename Representation, typename Period>
	std::chrono::duration<Representation, Period> PerformanceTimer::ElapsedTime() const
	{
		return m_end - m_start;
	}
}
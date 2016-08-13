#pragma once

#include <cstdint>
#include <exception>

namespace GraphicsEngine
{
	class Timer
	{
	public:
		explicit Timer(double millisecondsPerUpdate);

		void Reset();

		template<typename UpdateFunctionType, typename RenderFunctionType, typename ProcessInputFunctionType>
		bool UpdateAndRender(UpdateFunctionType& update, RenderFunctionType render, ProcessInputFunctionType processInput);

		double GetMillisecondsPerUpdate() const;
		double GetTotalMilliseconds() const;
		double GetDeltaMilliseconds() const;
		double GetLeftover() const;

	private:
		double TicksToMilliseconds(uint64_t ticks) const;
		static LARGE_INTEGER GetCurrentTick();
		
	private:
		const double m_millisecondsPerUpdate;

		uint64_t m_totalTicks;
		uint64_t m_deltaTicks;
		double m_lag;

		LARGE_INTEGER m_qpcFrequency;
		LARGE_INTEGER m_qpcPreviousTick;
		LARGE_INTEGER m_qpcCurrentTick;
	};

	inline Timer::Timer(double millisecondsPerUpdate) :
		m_millisecondsPerUpdate(millisecondsPerUpdate),
		m_totalTicks(0), 
		m_deltaTicks(0),
		m_lag(0)
	{
		if (!QueryPerformanceFrequency(&m_qpcFrequency))
		{
			throw std::exception();
		}
	}

	inline void Timer::Reset()
	{
		m_qpcPreviousTick = GetCurrentTick();
		m_lag = 0.0;
	}

	template <typename UpdateFunctionType, typename RenderFunctionType, typename ProcessInputFunctionType>
	bool Timer::UpdateAndRender(UpdateFunctionType& update, RenderFunctionType render, ProcessInputFunctionType processInput)
	{
		m_qpcCurrentTick = GetCurrentTick();
		auto deltaTicks = m_qpcCurrentTick.QuadPart - m_qpcPreviousTick.QuadPart;
		m_qpcPreviousTick = m_qpcCurrentTick;
		m_totalTicks += deltaTicks;

		auto deltaMilliseconds = TicksToMilliseconds(deltaTicks);
		m_lag += deltaMilliseconds;

		// Process input:
		if(!processInput())
			return false;

		while (m_lag >= m_millisecondsPerUpdate)
		{
			update(*this);
			m_lag -= m_millisecondsPerUpdate;
		}

		render(*this);

		return true;
	}

	inline double Timer::GetMillisecondsPerUpdate() const
	{
		return m_millisecondsPerUpdate;
	}

	inline double Timer::GetTotalMilliseconds() const
	{
		return TicksToMilliseconds(m_totalTicks);
	}

	inline double Timer::GetDeltaMilliseconds() const
	{
		return TicksToMilliseconds(m_deltaTicks);
	}

	inline double Timer::GetLeftover() const
	{
		return m_lag / m_millisecondsPerUpdate;
	}

	inline LARGE_INTEGER Timer::GetCurrentTick()
	{
		LARGE_INTEGER currentTime;
		if (!QueryPerformanceCounter(&currentTime))
		{
			throw std::exception();
		}

		return currentTime;
	}

	inline double Timer::TicksToMilliseconds(uint64_t ticks) const
	{
		return static_cast<double>((ticks * 1000)) / static_cast<double>(m_qpcFrequency.QuadPart);
	}
}

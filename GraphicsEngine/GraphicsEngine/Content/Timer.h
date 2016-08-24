#pragma once

#include <cstdint>

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

	template <typename UpdateFunctionType, typename RenderFunctionType, typename ProcessInputFunctionType>
	bool Timer::UpdateAndRender(UpdateFunctionType& update, RenderFunctionType render, ProcessInputFunctionType processInput)
	{
		m_qpcCurrentTick = GetCurrentTick();
		m_deltaTicks = m_qpcCurrentTick.QuadPart - m_qpcPreviousTick.QuadPart;
		m_qpcPreviousTick = m_qpcCurrentTick;
		m_totalTicks += m_deltaTicks;

		auto deltaMilliseconds = TicksToMilliseconds(m_deltaTicks);
		m_lag += deltaMilliseconds;

		// Process input:
		if (!processInput())
			return false;

		while (m_lag >= m_millisecondsPerUpdate)
		{
			update(*this);
			m_lag -= m_millisecondsPerUpdate;
		}

		render(*this);

		return true;
	}
}

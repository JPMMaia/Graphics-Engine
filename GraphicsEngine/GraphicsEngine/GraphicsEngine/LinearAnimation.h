#pragma once

#include "BaseAnimation.h"
#include "Common/Timer.h"

namespace GraphicsEngine
{
	template<typename Type>
	class LinearAnimation : public BaseAnimation
	{
	public:
		LinearAnimation(Type startValue, Type endValue, float startMilliseconds, float durationInMilliseconds) :
			m_startValue(startValue),
			m_endValue(endValue),
			m_startMilliseconds(startMilliseconds),
			m_durationInMilliseconds(durationInMilliseconds)
		{
		}

		void Update(const Common::Timer& timer) const override
		{
		}

		Type GetValue(const Common::Timer& timer) const
		{
			auto deltaMilliseconds = timer.GetTotalMilliseconds() - m_startMilliseconds;

			float blendFactor = deltaMilliseconds / m_durationInMilliseconds;

			return (1.0f - blendFactor) * m_startValue + blendFactor * m_endValue;
		}

		bool HasEnded(const Common::Timer& timer) const override
		{
			auto deltaMilliseconds = timer.GetTotalMilliseconds() - m_startMilliseconds;

			return deltaMilliseconds >= m_durationInMilliseconds;
		}

	private:
		Type m_startValue;
		Type m_endValue;
		float m_startMilliseconds;
		float m_durationInMilliseconds;
	};
}

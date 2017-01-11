#include "BaseAnimation.h"

using namespace GraphicsEngine;

BaseAnimation::BaseAnimation(float startInMilliseconds, float durationInMilliseconds) :
	m_startInMilliseconds(startInMilliseconds),
	m_durationInMilliseconds(durationInMilliseconds)
{
}

bool BaseAnimation::HasBegun(const Common::Timer& timer) const
{
	return timer.GetTotalMilliseconds() >= m_startInMilliseconds;
}
bool BaseAnimation::HasEnded(const Common::Timer& timer) const
{
	return timer.GetTotalMilliseconds() >= (m_startInMilliseconds + m_durationInMilliseconds);
}

float BaseAnimation::CalculateBlendFactor(const Common::Timer& timer) const
{
	auto deltaMilliseconds = timer.GetTotalMilliseconds() - m_startInMilliseconds;
	
	auto blendFactor = static_cast<float>(deltaMilliseconds) / m_durationInMilliseconds;
	if (blendFactor > m_durationInMilliseconds)
		blendFactor = 1.0f;

	return blendFactor;
}

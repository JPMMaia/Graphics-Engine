#include "stdafx.h"
#include "GeneralAnimation.h"

using namespace GraphicsEngine;

GeneralAnimation::GeneralAnimation(float startMilliseconds, float durationInMilliseconds, const UpdateFunctionType& updateFunction):
	m_startMilliseconds(startMilliseconds),
	m_durationInMilliseconds(durationInMilliseconds),
	m_updateFunction(updateFunction)
{
}

void GeneralAnimation::FixedUpdate(const Common::Timer& timer) const
{
	auto deltaMilliseconds = timer.GetTotalMilliseconds() - m_startMilliseconds;
	auto blendFactor = static_cast<float>(deltaMilliseconds) / m_durationInMilliseconds;

	m_updateFunction(timer, blendFactor);
}

bool GeneralAnimation::HasEnded(const Common::Timer& timer) const
{
	auto deltaMilliseconds = timer.GetTotalMilliseconds() - m_startMilliseconds;
	return deltaMilliseconds >= m_durationInMilliseconds;
}

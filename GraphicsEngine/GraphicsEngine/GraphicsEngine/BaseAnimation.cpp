#include "BaseAnimation.h"

using namespace GraphicsEngine;

BaseAnimation::BaseAnimation(AnimationType type, const std::string& typeName):
	m_type(type),
	m_typeName(typeName),
	m_startInMilliseconds(0.0f),
	m_durationInMilliseconds(0.0f)
{
}

BaseAnimation::BaseAnimation(AnimationType type, const std::string& typeName, float startInMilliseconds, float durationInMilliseconds):
	m_type(type),
	m_typeName(typeName),
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

AnimationType BaseAnimation::GetType() const
{
	return m_type;
}

const std::string& BaseAnimation::GetTypeName() const
{
	return m_typeName;
}

float BaseAnimation::GetStartInMilliseconds() const
{
	return m_startInMilliseconds;
}
void BaseAnimation::SetStartInMilliseconds(float startInMilliseconds)
{
	m_startInMilliseconds = startInMilliseconds;
}
float BaseAnimation::GetDurationInMilliseconds() const
{
	return m_durationInMilliseconds;
}
void BaseAnimation::SetDurationInMilliseconds(float durationInMilliseconds)
{
	m_durationInMilliseconds = durationInMilliseconds;
}

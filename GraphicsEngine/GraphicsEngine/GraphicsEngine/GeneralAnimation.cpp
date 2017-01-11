#include "stdafx.h"
#include "GeneralAnimation.h"

using namespace GraphicsEngine;

GeneralAnimation::GeneralAnimation(float startInMilliseconds, float durationInMilliseconds, const UpdateFunctionType& updateFunction) :
	BaseAnimation(startInMilliseconds, durationInMilliseconds),
	m_updateFunction(updateFunction)
{
}

void GeneralAnimation::FixedUpdate(const Common::Timer& timer) const
{
	auto blendFactor = CalculateBlendFactor(timer);

	m_updateFunction(timer, blendFactor);
}


#include "stdafx.h"
#include "GeneralAnimation.h"
#include "BaseAnimation.h"
#include "Common/NotImplementedException.h"

using namespace GraphicsEngine;

GeneralAnimation::GeneralAnimation(float startInMilliseconds, float durationInMilliseconds, const UpdateFunctionType& updateFunction) :
	BaseAnimation(AnimationType::GeneralAnimation, "GeneralAnimation", startInMilliseconds, durationInMilliseconds),
	m_updateFunction(updateFunction)
{
}

void GeneralAnimation::FixedUpdate(const Common::Timer& timer) const
{
	auto blendFactor = CalculateBlendFactor(timer);

	m_updateFunction(timer, blendFactor);
}

nlohmann::json GeneralAnimation::ToJson() const
{
	throw Common::NotImplementedException();
}

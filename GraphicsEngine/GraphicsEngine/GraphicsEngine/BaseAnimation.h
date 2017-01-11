#pragma once

#include "Common/Timer.h"

namespace GraphicsEngine
{
	class BaseAnimation
	{
	public:
		BaseAnimation(float startInMilliseconds, float durationInMilliseconds);
		virtual ~BaseAnimation() = default;

		virtual void FixedUpdate(const Common::Timer& timer) const = 0;
		
		virtual bool HasBegun(const Common::Timer& timer) const;
		virtual bool HasEnded(const Common::Timer& timer) const;

		float CalculateBlendFactor(const Common::Timer& timer) const;

	private:
		float m_startInMilliseconds;
		float m_durationInMilliseconds;
	};
}

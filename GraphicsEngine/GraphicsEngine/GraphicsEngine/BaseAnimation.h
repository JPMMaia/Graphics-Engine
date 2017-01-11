#pragma once

#include "Common/Timer.h"

namespace GraphicsEngine
{
	class BaseAnimation
	{
	public:
		virtual ~BaseAnimation() = default;

		virtual void Update(const Common::Timer& timer) const = 0;
		virtual bool HasEnded(const Common::Timer& timer) const = 0;
	};
}

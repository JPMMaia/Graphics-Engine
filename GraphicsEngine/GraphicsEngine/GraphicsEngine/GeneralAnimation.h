#pragma once

#include "BaseAnimation.h"

#include <functional>

namespace GraphicsEngine
{
	class GeneralAnimation : public BaseAnimation
	{
	public:
		using UpdateFunctionType = std::function<void(const Common::Timer&, float)>;

	public:
		GeneralAnimation(float startInMilliseconds, float durationInMilliseconds, const UpdateFunctionType& updateFunction);

		void FixedUpdate(const Common::Timer& timer) const override;

	private:
		UpdateFunctionType m_updateFunction;
	};
}

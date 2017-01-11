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
		GeneralAnimation(float startMilliseconds, float durationInMilliseconds, const UpdateFunctionType& updateFunction);

		void Update(const Common::Timer& timer) const override;
		bool HasEnded(const Common::Timer& timer) const override;

	private:
		UpdateFunctionType m_updateFunction;
		float m_startMilliseconds;
		float m_durationInMilliseconds;
	};
}

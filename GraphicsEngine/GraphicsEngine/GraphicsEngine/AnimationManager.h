#pragma once

#include "Common/Timer.h"
#include "BaseAnimation.h"

namespace GraphicsEngine
{
	class AnimationManager
	{
	public:
		AnimationManager() = default;

		void FixedUpdate(const Common::Timer& timer);

		void AddAnimation(std::unique_ptr<BaseAnimation>&& pAnimation);

	private:
		std::vector<std::unique_ptr<BaseAnimation>> m_animations;
	};
}

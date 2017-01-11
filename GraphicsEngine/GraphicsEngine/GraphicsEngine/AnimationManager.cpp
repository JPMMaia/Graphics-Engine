#include "stdafx.h"
#include "AnimationManager.h"

using namespace GraphicsEngine;

void AnimationManager::FixedUpdate(const Common::Timer& timer)
{
	for(auto& pAnimation : m_animations)
	{
		if(pAnimation->HasBegun(timer))
			pAnimation->FixedUpdate(timer);
	}

	auto eraser = [&timer](const std::unique_ptr<BaseAnimation>& pAnimation)
	{
		return pAnimation->HasEnded(timer);
	};
	m_animations.erase(std::remove_if(m_animations.begin(), m_animations.end(), eraser), m_animations.end());
}

void AnimationManager::AddAnimation(std::unique_ptr<BaseAnimation>&& pAnimation)
{
	m_animations.push_back(std::move(pAnimation));
}

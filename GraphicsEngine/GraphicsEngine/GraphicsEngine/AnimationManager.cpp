#include "stdafx.h"
#include "AnimationManager.h"

using namespace GraphicsEngine;

void AnimationManager::Update(const Common::Timer& timer)
{
	for(auto& pAnimation : m_animations)
	{
		pAnimation->Update(timer);
	}

	auto eraser = [&timer](const std::unique_ptr<BaseAnimation>& pAnimation)
	{
		return pAnimation->HasEnded(timer);
	};
	std::remove_if(m_animations.begin(), m_animations.end(), eraser);
}

void AnimationManager::AddAnimation(std::unique_ptr<BaseAnimation>&& pAnimation)
{
	m_animations.push_back(std::move(pAnimation));
}

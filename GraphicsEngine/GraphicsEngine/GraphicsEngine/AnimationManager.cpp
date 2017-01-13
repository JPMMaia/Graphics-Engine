#include "stdafx.h"
#include "AnimationManager.h"

#include <fstream>
#include "nlohmann/json/json.hpp"
#include "FogAnimation.h"
#include "CameraAnimation.h"
#include "Graphics.h"
#include "Common/NotImplementedException.h"

using namespace Common;
using namespace GraphicsEngine;
using namespace std;

AnimationManager::AnimationManager(Graphics& graphics, const std::wstring& filename) :
	m_graphics(graphics),
	m_filename(filename)
{
	LoadFromFile();
}
AnimationManager::~AnimationManager()
{
	SaveToFile();
}

void AnimationManager::FixedUpdate(const Common::Timer& timer)
{
	for (auto& pAnimation : m_animations)
	{
		if (pAnimation->HasBegun(timer))
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
void AnimationManager::AddAnimation(std::unique_ptr<CameraAnimation>&& pAnimation)
{
	auto finalTime = pAnimation->GetStartInMilliseconds() + pAnimation->GetDurationInMilliseconds();
	if(m_lastCameraFinalTime < finalTime)
	{
		m_lastCameraFinalTime = finalTime;
		m_lastCameraPosition = pAnimation->GetFinalPosition();
		m_lastCameraRotationQuaternion = pAnimation->GetFinalRotationQuaternion();
	}

	m_animations.push_back(std::move(pAnimation));
}

DirectX::XMVECTOR AnimationManager::GetLastCameraPosition() const
{
	return m_lastCameraPosition;
}
DirectX::XMVECTOR AnimationManager::GetLastCameraRotationQuaternion() const
{
	return m_lastCameraRotationQuaternion;
}
float AnimationManager::GetLastCameraFinalTime() const
{
	return m_lastCameraFinalTime;
}

void AnimationManager::SaveToFile() const
{
	std::ofstream fileStream(m_filename, ios::out);

	auto fileJson(nlohmann::json::array({}));

	for (const auto& animation : m_animations)
	{
		fileJson.push_back(animation->ToJson());
	}

	fileStream << fileJson;
}

void AnimationManager::LoadFromFile()
{
	std::ifstream fileStream(m_filename, ios::in);
	if (!fileStream.good())
		return;

	nlohmann::json fileJson;
	fileStream >> fileJson;

	if (!fileJson.is_array())
		ThrowEngineException(L"Expecting an array");

	auto& camera = *m_graphics.GetCamera();

	for (const auto& animationJson : fileJson)
	{
		auto name = animationJson.at("Name").get<string>();

		if (name == "CameraAnimation")
			AddAnimation(std::make_unique<CameraAnimation>(CameraAnimation::FromJson(animationJson, camera)));
		else if (name == "FogAnimation")
			AddAnimation(std::make_unique<FogAnimation>(FogAnimation::FromJson(animationJson, m_graphics)));
		else
			throw NotImplementedException();
	}
}
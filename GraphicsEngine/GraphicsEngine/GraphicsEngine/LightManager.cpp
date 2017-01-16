#include "stdafx.h"
#include "LightManager.h"

using namespace GraphicsEngine;

void LightManager::AddLight(std::unique_ptr<Light>&& light)
{
	switch (light->GetType())
	{
	case Light::Type::Directional:
		m_directionalLights.push_back(light.get());
		break;

	case Light::Type::Point:
		m_pointLights.push_back(light.get());
		break;

	case Light::Type::Spot:
		m_spotLights.push_back(light.get());
		break;

	default:
		break;
	}

	if (light->CastShadows())
		m_castShadowsLights.push_back(light.get());

	m_lights.push_back(std::move(light));
}

const DirectX::XMFLOAT4& LightManager::GetAmbientLight() const
{
	return m_ambientLight;
}
void LightManager::SetAmbientLight(const DirectX::XMFLOAT4& ambientLight)
{
	m_ambientLight = ambientLight;
}

const std::vector<Light*>& LightManager::GetCastShadowsLights() const
{
	return m_castShadowsLights;
}

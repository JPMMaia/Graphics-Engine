#include "stdafx.h"
#include "LightManager.h"

using namespace GraphicsEngine;

void LightManager::AddDirectionalLight(const DirectX::XMFLOAT3& strength, const DirectX::XMFLOAT3& direction)
{
	ShaderBufferTypes::LightData light = {};
	light.Strength = strength;
	light.Direction = direction;

	m_directionalLights.emplace_back(std::move(light));
}
void LightManager::AddPointLight(const DirectX::XMFLOAT3& strength, float falloffStart, float falloffEnd, const DirectX::XMFLOAT3& position)
{
	ShaderBufferTypes::LightData light = {};
	light.Strength = strength;
	light.FalloffStart = falloffStart;
	light.FalloffEnd = falloffEnd;
	light.Position = position;

	m_pointLights.emplace_back(std::move(light));
}
void LightManager::AddSpotLight(const DirectX::XMFLOAT3& strength, float falloffStart, const DirectX::XMFLOAT3& direction, float falloffEnd, const DirectX::XMFLOAT3& position, float spotPower)
{
	ShaderBufferTypes::LightData light = {};
	light.Strength = strength;
	light.FalloffStart = falloffStart;
	light.Direction = direction;
	light.FalloffEnd = falloffEnd;
	light.Position = position;
	light.SpotPower = spotPower;

	m_spotLights.emplace_back(std::move(light));
}

const DirectX::XMFLOAT4& LightManager::GetAmbientLight() const
{
	return m_ambientLight;
}
void LightManager::SetAmbientLight(const DirectX::XMFLOAT4& ambientLight)
{
	m_ambientLight = ambientLight;
}

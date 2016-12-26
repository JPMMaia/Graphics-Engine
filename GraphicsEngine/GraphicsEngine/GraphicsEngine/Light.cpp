#include "stdafx.h"
#include "Light.h"

using namespace DirectX;
using namespace GraphicsEngine;

Light Light::CreateDirectionalLight(const DirectX::XMFLOAT3& strength, const DirectX::XMFLOAT3& direction)
{
	Light light;
	light.m_type = Type::Directional;
	light.m_lightData.Strength = strength;
	light.m_lightData.Direction = direction;
	light.m_castShadows = false;

	return light;
}
Light Light::CreateDirectionalCastShadowsLight(const DirectX::XMFLOAT3& strength, const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
	Light light;
	light.m_type = Type::Directional;
	light.m_lightData.Strength = strength;
	light.m_lightData.Direction = direction;
	light.m_lightData.Position = position;
	light.m_castShadows = true;

	return light;
}
Light Light::CreatePointLight(const DirectX::XMFLOAT3& strength, float falloffStart, float falloffEnd, const DirectX::XMFLOAT3& position)
{
	Light light;
	light.m_lightData.Strength = strength;
	light.m_lightData.FalloffStart = falloffStart;
	light.m_lightData.FalloffEnd = falloffEnd;
	light.m_lightData.Position = position;

	return light;
}
Light Light::CreateSpotLight(const DirectX::XMFLOAT3& strength, float falloffStart, const DirectX::XMFLOAT3& direction, float falloffEnd, const DirectX::XMFLOAT3& position, float spotPower)
{
	Light light;
	light.m_lightData.Strength = strength;
	light.m_lightData.FalloffStart = falloffStart;
	light.m_lightData.Direction = direction;
	light.m_lightData.FalloffEnd = falloffEnd;
	light.m_lightData.Position = position;
	light.m_lightData.SpotPower = spotPower;

	return light;
}

XMFLOAT4X4 Light::GetViewMatrix() const
{
	auto eyePosition = XMLoadFloat3(&m_lightData.Position);
	auto focusPosition = XMLoadFloat3(&m_lightData.Position) + XMLoadFloat3(&m_lightData.Direction);
	auto upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	
	XMFLOAT4X4 viewMatrix;
	XMStoreFloat4x4(&viewMatrix, XMMatrixLookAtLH(eyePosition, focusPosition, upDirection));

	return viewMatrix;
}
DirectX::XMFLOAT4X4 Light::GetOrthographicMatrix(float viewWidth, float viewHeight, float nearZ, float farZ)
{
	XMFLOAT4X4 orthographicMatrix;
	XMStoreFloat4x4(&orthographicMatrix, XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ));

	return orthographicMatrix;
}

Light::Type Light::GetType() const
{
	return m_type;
}
const ShaderBufferTypes::LightData& Light::GetLightData() const
{
	return m_lightData;
}
bool Light::CastShadows() const
{
	return m_castShadows;
}

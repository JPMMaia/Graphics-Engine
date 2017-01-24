#include "stdafx.h"
#include "Light.h"

using namespace DirectX;
using namespace GraphicsEngine;

Light Light::CreateDirectionalLight(const DirectX::XMFLOAT3& strength, const DirectX::XMFLOAT3& direction, bool castShadows)
{
	Light light;
	light.m_type = Type::Directional;
	light.m_lightData.Strength = strength;
	light.m_lightData.Direction = direction;
	light.m_castShadows = castShadows;

	return light;
}
Light Light::CreatePointLight(const DirectX::XMFLOAT3& strength, float falloffStart, float falloffEnd, const DirectX::XMFLOAT3& position)
{
	Light light;
	light.m_type = Type::Point;
	light.m_lightData.Strength = strength;
	light.m_lightData.FalloffStart = falloffStart;
	light.m_lightData.FalloffEnd = falloffEnd;
	light.m_lightData.Position = position;

	return light;
}
Light Light::CreateSpotLight(const DirectX::XMFLOAT3& strength, float falloffStart, const DirectX::XMFLOAT3& direction, float falloffEnd, const DirectX::XMFLOAT3& position, float spotPower)
{
	Light light;
	light.m_type = Type::Spot;
	light.m_lightData.Strength = strength;
	light.m_lightData.FalloffStart = falloffStart;
	light.m_lightData.Direction = direction;
	light.m_lightData.FalloffEnd = falloffEnd;
	light.m_lightData.Position = position;
	light.m_lightData.SpotPower = spotPower;

	return light;
}

void Light::UpdateMatrices(const BoundingSphere& sceneBounds, FXMVECTOR cameraPosition)
{
	// Calculate the light position based on the direction of the light and the scene bounds:
	auto lightDirection = XMLoadFloat3(&m_lightData.Direction);
	auto lightPosition = cameraPosition - 50.0f * lightDirection;
	XMStoreFloat3(&m_lightData.Position, lightPosition);

	// Calculate the light view matrix:
	auto targetPosition = cameraPosition;
	auto upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_viewMatrix = XMMatrixLookAtLH(lightPosition, targetPosition, upDirection);

	// Transform center of scene to light view space:
	XMFLOAT3 sceneCenterLightSpace;
	XMStoreFloat3(&sceneCenterLightSpace, XMVector3TransformCoord(targetPosition, m_viewMatrix));

	// Calculate the light orthographic projection matrix:
	auto left = -100.0f;
	auto right = 100.0f;
	auto bottom = -100.0f;
	auto top = 100.0f;
	auto nearZ = 1.0f;
	auto farZ = 500.0f;
	m_projectionMatrix = XMMatrixOrthographicOffCenterLH(left, right, bottom, top, nearZ, farZ);

	// Transform from NDC space to texture space:
	auto lightTextureMatrix = XMMatrixSet(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f
	);

	m_shadowMatrix = m_viewMatrix * m_projectionMatrix * lightTextureMatrix;
}
void Light::RotateRollPitchYaw(float pitchRadians, float yawRadians, float rollRadians)
{
	auto newDirection = XMVector3Rotate(XMLoadFloat3(&m_lightData.Direction), XMQuaternionRotationRollPitchYaw(pitchRadians, yawRadians, rollRadians));
	XMStoreFloat3(&m_lightData.Direction, newDirection);
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

const XMMATRIX& Light::GetViewMatrix() const
{
	return m_viewMatrix;
}
const XMMATRIX& Light::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}
const XMMATRIX& Light::GetShadowMatrix() const
{
	return m_shadowMatrix;
}
void Light::SetStrength(const DirectX::XMFLOAT3& strength)
{
	m_lightData.Strength = strength;
}
void Light::SetDirection(const DirectX::XMFLOAT3& direction)
{
	m_lightData.Direction = direction;
}
void Light::SetPosition(const DirectX::XMFLOAT3& position)
{
	m_lightData.Position = position;
}

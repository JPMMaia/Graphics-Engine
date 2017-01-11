#include "stdafx.h"
#include "CameraAnimation.h"
#include "Camera.h"
#include "Common/MathHelper.h"

using namespace DirectX;
using namespace GraphicsEngine;

CameraAnimation::CameraAnimation(Camera& camera, float startInMilliseconds, float durationInMilliseconds, DirectX::CXMVECTOR initialPosition, DirectX::CXMVECTOR finalPosition, DirectX::CXMVECTOR initialRotationQuaternion, DirectX::CXMVECTOR finalRotationQuaternion) :
	BaseAnimation(startInMilliseconds, durationInMilliseconds),
	m_camera(camera),
	m_initialPosition(initialPosition),
	m_finalPosition(finalPosition),
	m_initialRotationQuaternion(initialRotationQuaternion),
	m_finalRotationQuaternion(finalRotationQuaternion)
{
}

void CameraAnimation::FixedUpdate(const Common::Timer& timer) const
{
	auto blendFactor = CalculateBlendFactor(timer);

	auto position = MathHelper::LinearInterpolate(m_initialPosition, m_finalPosition, blendFactor);
	auto rotation = MathHelper::LinearInterpolate(m_initialRotationQuaternion, m_finalRotationQuaternion, blendFactor);

	m_camera.SetPosition(position);
	m_camera.SetRotationQuaternion(rotation);
}


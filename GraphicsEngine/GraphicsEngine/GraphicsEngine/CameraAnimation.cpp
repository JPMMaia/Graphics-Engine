#include "stdafx.h"
#include "CameraAnimation.h"
#include "Camera.h"
#include "Common/MathHelper.h"

using namespace DirectX;
using namespace GraphicsEngine;

CameraAnimation::CameraAnimation(Camera& camera, float startMilliseconds, float durationInMilliseconds, DirectX::CXMVECTOR initialPosition, DirectX::CXMVECTOR finalPosition, DirectX::CXMVECTOR initialRotationQuaternion, DirectX::CXMVECTOR finalRotationQuaternion) :
	m_camera(camera),
	m_startMilliseconds(startMilliseconds),
	m_durationInMilliseconds(durationInMilliseconds),
	m_initialPosition(initialPosition),
	m_finalPosition(finalPosition),
	m_initialRotationQuaternion(initialRotationQuaternion),
	m_finalRotationQuaternion(finalRotationQuaternion)
{
}

void CameraAnimation::FixedUpdate(const Common::Timer& timer) const
{
	auto deltaMilliseconds = timer.GetTotalMilliseconds() - m_startMilliseconds;
	auto blendFactor = static_cast<float>(deltaMilliseconds) / m_durationInMilliseconds;

	auto position = MathHelper::LinearInterpolate(m_initialPosition, m_finalPosition, blendFactor);
	auto rotation = MathHelper::LinearInterpolate(m_initialRotationQuaternion, m_finalRotationQuaternion, blendFactor);

	m_camera.SetPosition(position);
	m_camera.SetRotationQuaternion(rotation);
}

bool CameraAnimation::HasEnded(const Common::Timer& timer) const
{
	auto deltaMilliseconds = timer.GetTotalMilliseconds() - m_startMilliseconds;
	return deltaMilliseconds >= m_durationInMilliseconds;
}

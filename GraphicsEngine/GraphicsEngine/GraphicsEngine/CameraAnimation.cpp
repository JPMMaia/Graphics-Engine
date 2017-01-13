#include "stdafx.h"
#include "CameraAnimation.h"
#include "BaseAnimation.h"
#include "Common/MathHelper.h"
#include "JsonHelper.h"
#include "Common/MathHelper.h"

using namespace DirectX;
using namespace GraphicsEngine;

CameraAnimation::CameraAnimation(Camera& camera) :
	BaseAnimation(AnimationType::CameraAnimation, "CameraAnimation"),
	m_camera(camera)
{
}
CameraAnimation::CameraAnimation(Camera& camera, float startInMilliseconds, float durationInMilliseconds, DirectX::CXMVECTOR initialPosition, DirectX::CXMVECTOR finalPosition, DirectX::CXMVECTOR initialRotationQuaternion, DirectX::CXMVECTOR finalRotationQuaternion) :
	BaseAnimation(AnimationType::CameraAnimation, "CameraAnimation", startInMilliseconds, durationInMilliseconds),
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
	auto rotationQuaternion = XMQuaternionSlerp(m_initialRotationQuaternion, m_finalRotationQuaternion, blendFactor);

	m_camera.SetPosition(position);
	m_camera.SetRotationQuaternion(rotationQuaternion);
}

nlohmann::json CameraAnimation::ToJson() const
{
	return{
		{ "Name", "CameraAnimation" },
		{ "TimeParameters",
			{
				{ "StartInMilliseconds", GetStartInMilliseconds() },
				{ "DurationInMilliseconds", GetDurationInMilliseconds() },
			}
		},
		{ "SpecificParameters",
			{
				{ "Position", { JsonHelper::XMVectorToJson(m_initialPosition), JsonHelper::XMVectorToJson(m_finalPosition) } },
				{ "RotationQuaternion", { JsonHelper::XMVectorToJson(m_initialRotationQuaternion), JsonHelper::XMVectorToJson(m_finalRotationQuaternion) } },
			}
		}
	};
}

CameraAnimation CameraAnimation::FromJson(const nlohmann::json& objectJson, Camera& camera)
{
	CameraAnimation object(camera);

	{
		auto timeParametersJson = objectJson.at("TimeParameters");
		object.SetStartInMilliseconds(timeParametersJson.at("StartInMilliseconds").get<float>());
		object.SetDurationInMilliseconds(timeParametersJson.at("DurationInMilliseconds").get<float>());
	}

	{
		auto specificParametersJson = objectJson.at("SpecificParameters");

		{
			auto positionJsonArray = specificParametersJson.at("Position");
			object.SetInitialPosition(JsonHelper::JsonToXMVector(positionJsonArray[0]));
			object.SetFinalPosition(JsonHelper::JsonToXMVector(positionJsonArray[1]));
		}

		{
			auto rotationQuaternionJsonArray = specificParametersJson.at("RotationQuaternion");
			object.SetInitialRotationQuaternion(JsonHelper::JsonToXMVector(rotationQuaternionJsonArray[0]));
			object.SetFinalRotationQuaternion(JsonHelper::JsonToXMVector(rotationQuaternionJsonArray[1]));
		}
	}

	return object;
}

DirectX::XMVECTOR CameraAnimation::GetInitialPosition() const
{
	return m_initialPosition;
}

void CameraAnimation::SetInitialPosition(const DirectX::XMVECTOR& initialPosition)
{
	m_initialPosition = initialPosition;
}

DirectX::XMVECTOR CameraAnimation::GetFinalPosition() const
{
	return m_finalPosition;
}

void CameraAnimation::SetFinalPosition(const DirectX::XMVECTOR& finalPosition)
{
	m_finalPosition = finalPosition;
}

DirectX::XMVECTOR CameraAnimation::GetInitialRotationQuaternion() const
{
	return m_initialRotationQuaternion;
}

void CameraAnimation::SetInitialRotationQuaternion(const DirectX::XMVECTOR& initialRotationQuaternion)
{
	m_initialRotationQuaternion = initialRotationQuaternion;
}

DirectX::XMVECTOR CameraAnimation::GetFinalRotationQuaternion() const
{
	return m_finalRotationQuaternion;
}

void CameraAnimation::SetFinalRotationQuaternion(const DirectX::XMVECTOR& finalRotationQuaternion)
{
	m_finalRotationQuaternion = finalRotationQuaternion;
}

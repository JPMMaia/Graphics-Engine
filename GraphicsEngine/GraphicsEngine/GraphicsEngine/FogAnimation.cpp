#include "stdafx.h"
#include "FogAnimation.h"
#include "BaseAnimation.h"
#include "Graphics.h"
#include "JsonHelper.h"
#include "Common/MathHelper.h"

using namespace DirectX;
using namespace GraphicsEngine;

FogAnimation::FogAnimation(Graphics& graphics) :
	BaseAnimation(AnimationType::FogAnimation, "FogAnimation"),
	m_graphics(graphics),
	m_startFogStart(0),
	m_finalFogStart(0),
	m_startFogRange(0),
	m_finalFogRange(0)
{
}

FogAnimation::FogAnimation(float startInMilliseconds, float durationInMilliseconds, Graphics& graphics, float startFogStart, float finalFogStart, float startFogRange, float finalFogRange, const DirectX::XMVECTOR& startColor, const DirectX::XMVECTOR& finalColor) :
	BaseAnimation(AnimationType::FogAnimation, "FogAnimation", startInMilliseconds, durationInMilliseconds),
	m_graphics(graphics),
	m_startFogStart(startFogStart),
	m_finalFogStart(finalFogStart),
	m_startFogRange(startFogRange),
	m_finalFogRange(finalFogRange),
	m_startColor(startColor),
	m_finalColor(finalColor)
{
}

void FogAnimation::FixedUpdate(const Common::Timer& timer) const
{
	auto blendFactor = CalculateBlendFactor(timer);

	XMFLOAT4 fogColor;
	XMStoreFloat4(&fogColor, MathHelper::LinearInterpolate(m_startColor, m_finalColor, blendFactor));

	auto fogStart = MathHelper::LinearInterpolate(m_startFogStart, m_finalFogStart, blendFactor);
	auto fogRange = MathHelper::LinearInterpolate(m_startFogRange, m_finalFogRange, blendFactor);

	m_graphics.SetFogColor(fogColor);
	m_graphics.SetFogDistanceParameters(fogStart, fogRange);
}

bool FogAnimation::HasBegun(const Common::Timer& timer) const
{
	return BaseAnimation::HasBegun(timer);
}
bool FogAnimation::HasEnded(const Common::Timer& timer) const
{
	return BaseAnimation::HasEnded(timer);
}

nlohmann::json FogAnimation::ToJson() const
{
	return{
		{ "Name", "FogAnimation" },
		{ "TimeParameters",
			{
				{ "StartInMilliseconds", GetStartInMilliseconds() },
				{ "DurationInMilliseconds", GetDurationInMilliseconds() },
			}
		},
		{ "SpecificParameters",
			{
				{ "FogStart", { m_startFogStart, m_finalFogStart } },
				{ "FogRange", { m_startFogRange, m_finalFogRange } },
				{ "FogColor", { JsonHelper::XMVectorToJson(m_startColor), JsonHelper::XMVectorToJson(m_finalColor) } }
			}
		}
	};
}

FogAnimation FogAnimation::FromJson(const nlohmann::json& objectJson, Graphics& graphics)
{
	FogAnimation object(graphics);

	{
		auto timeParametersJson = objectJson.at("TimeParameters");
		object.SetStartInMilliseconds(timeParametersJson.at("StartInMilliseconds").get<float>());
		object.SetDurationInMilliseconds(timeParametersJson.at("DurationInMilliseconds").get<float>());
	}

	{
		auto specificParametersJson = objectJson.at("SpecificParameters");

		{
			auto fogStartJsonArray = specificParametersJson.at("FogStart");
			object.SetStartFogStart(fogStartJsonArray[0].get<float>());
			object.SetFinalFogStart(fogStartJsonArray[1].get<float>());
		}

		{
			auto fogRangeJsonArray = specificParametersJson.at("FogRange");
			object.SetStartFogRange(fogRangeJsonArray[0].get<float>());
			object.SetFinalFogRange(fogRangeJsonArray[1].get<float>());
		}

		{
			auto fogColorJsonArray = specificParametersJson.at("FogColor");
			object.SetStartColor(JsonHelper::JsonToXMVector(fogColorJsonArray[0]));
			object.SetFinalColor(JsonHelper::JsonToXMVector(fogColorJsonArray[1]));
		}
	}

	return object;
}

float FogAnimation::GetStartFogStart() const
{
	return m_startFogStart;
}

void FogAnimation::SetStartFogStart(float startFogStart)
{
	m_startFogStart = startFogStart;
}

float FogAnimation::GetFinalFogStart() const
{
	return m_finalFogStart;
}

void FogAnimation::SetFinalFogStart(float finalFogStart)
{
	m_finalFogStart = finalFogStart;
}

float FogAnimation::GetStartFogRange() const
{
	return m_startFogRange;
}

void FogAnimation::SetStartFogRange(float startFogRange)
{
	m_startFogRange = startFogRange;
}

float FogAnimation::GetFinalFogRange() const
{
	return m_finalFogRange;
}

void FogAnimation::SetFinalFogRange(float finalFogRange)
{
	m_finalFogRange = finalFogRange;
}

DirectX::XMVECTOR FogAnimation::GetStartColor() const
{
	return m_startColor;
}

void FogAnimation::SetStartColor(const DirectX::XMVECTOR& startColor)
{
	m_startColor = startColor;
}

DirectX::XMVECTOR FogAnimation::GetFinalColor() const
{
	return m_finalColor;
}

void FogAnimation::SetFinalColor(const DirectX::XMVECTOR& finalColor)
{
	m_finalColor = finalColor;
}

#include "stdafx.h"
#include "KeyAnimation.h"
#include "DXInputHandler.h"
#include "JsonHelper.h"

using namespace GraphicsEngine;

KeyAnimation::KeyAnimation(DXInputHandler& inputHandler) :
	BaseAnimation(AnimationType::KeyAnimation, "KeyAnimation"),
	m_inputHandler(inputHandler),
	m_key(0)
{
}
KeyAnimation::KeyAnimation(DXInputHandler& inputHandler, uint8_t key, float startInMilliseconds) :
	BaseAnimation(AnimationType::KeyAnimation, "KeyAnimation", startInMilliseconds, 0.0f),
	m_inputHandler(inputHandler),
	m_key(key)
{
}

void KeyAnimation::FixedUpdate(const Common::Timer& timer) const
{
	m_inputHandler.PressKey(m_key);
}

uint8_t KeyAnimation::GetKey() const
{
	return m_key;
}
void KeyAnimation::SetKey(uint8_t key)
{
	m_key = key;
}

nlohmann::json KeyAnimation::ToJson() const
{
	return{
		{ "Name", "KeyAnimation" },
		{ "TimeParameters",
		{
			{ "StartInMilliseconds", GetStartInMilliseconds() }
		}
		},
		{ "SpecificParameters",
		{
			{ "Key", { m_key } }
		}
		}
	};
}

KeyAnimation KeyAnimation::FromJson(const nlohmann::json& objectJson, DXInputHandler& inputHandler)
{
	KeyAnimation object(inputHandler);

	{
		auto timeParametersJson = objectJson.at("TimeParameters");
		object.SetStartInMilliseconds(timeParametersJson.at("StartInMilliseconds").get<float>());
		object.SetDurationInMilliseconds(0.0f);
	}

	{
		auto specificParametersJson = objectJson.at("SpecificParameters");

		{
			auto keyJsonArray = specificParametersJson.at("Key");
			object.SetKey(keyJsonArray.get<uint8_t>());
		}
	}

	return object;
}

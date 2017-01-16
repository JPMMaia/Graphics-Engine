#pragma once

#include "Common/Timer.h"
#include "AnimationType.h"

#include <string>
#include "nlohmann/json/json.hpp"

namespace GraphicsEngine
{
	class BaseAnimation
	{
	public:
		BaseAnimation(AnimationType type, const std::string& typeName);

		BaseAnimation(AnimationType type, const std::string& typeName, float startInMilliseconds, float durationInMilliseconds);
		virtual ~BaseAnimation() = default;

		virtual void FixedUpdate(const Common::Timer& timer) const = 0;

		virtual bool HasBegun(const Common::Timer& timer) const;

		virtual bool HasEnded(const Common::Timer& timer) const;

		float CalculateBlendFactor(const Common::Timer& timer) const;

		virtual nlohmann::json ToJson() const = 0;

		AnimationType GetType() const;

		const std::string& GetTypeName() const;


		float GetStartInMilliseconds() const;
		void SetStartInMilliseconds(float startInMilliseconds);
		float GetDurationInMilliseconds() const;
		void SetDurationInMilliseconds(float durationInMilliseconds);

	private:
		AnimationType m_type;
		std::string m_typeName;
		float m_startInMilliseconds;
		float m_durationInMilliseconds;
	};
}

#pragma once

#include "BaseAnimation.h"

#include <DirectXMath.h>

namespace GraphicsEngine
{
	class Graphics;

	__declspec(align(16))
	class FogAnimation : public BaseAnimation
	{
	private:
		explicit FogAnimation(Graphics& graphics);

	public:
		FogAnimation(float startInMilliseconds, float durationInMilliseconds, Graphics& graphics, float startFogStart, float finalFogStart, float startFogRange, float finalFogRange, const DirectX::XMVECTOR& startColor, const DirectX::XMVECTOR& finalColor);

		void FixedUpdate(const Common::Timer& timer) const override;

		bool HasBegun(const Common::Timer& timer) const override;
		bool HasEnded(const Common::Timer& timer) const override;

		nlohmann::json ToJson() const override;
		static FogAnimation FromJson(const nlohmann::json& objectJson, Graphics& graphics);

		float GetStartFogStart() const;
		void SetStartFogStart(float startFogStart);
		float GetFinalFogStart() const;
		void SetFinalFogStart(float finalFogStart);
		float GetStartFogRange() const;
		void SetStartFogRange(float startFogRange);
		float GetFinalFogRange() const;
		void SetFinalFogRange(float finalFogRange);
		DirectX::XMVECTOR GetStartColor() const;
		void SetStartColor(const DirectX::XMVECTOR& startColor);
		DirectX::XMVECTOR GetFinalColor() const;
		void SetFinalColor(const DirectX::XMVECTOR& finalColor);

	private:
		Graphics& m_graphics;
		float m_startFogStart;
		float m_finalFogStart;
		float m_startFogRange;
		float m_finalFogRange;
		DirectX::XMVECTOR m_startColor;
		DirectX::XMVECTOR m_finalColor;
	};
}

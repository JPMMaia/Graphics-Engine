#pragma once

#include "BaseAnimation.h"

#include <DirectXMath.h>

namespace GraphicsEngine
{
	class Graphics;

	__declspec(align(16))
	class FogAnimation : public BaseAnimation
	{
	public:
		FogAnimation(float startInMilliseconds, float durationInMilliseconds, Graphics& graphics, float startFogStart, float finalFogStart, float startFogRange, float finalFogRange, const DirectX::XMVECTOR& startColor, const DirectX::XMVECTOR& finalColor);

		void FixedUpdate(const Common::Timer& timer) const override;

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

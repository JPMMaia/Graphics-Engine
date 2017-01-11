#include "stdafx.h"
#include "FogAnimation.h"
#include "Graphics.h"

using namespace DirectX;
using namespace GraphicsEngine;

FogAnimation::FogAnimation(float startInMilliseconds, float durationInMilliseconds, Graphics& graphics, float startFogStart, float finalFogStart, float startFogRange, float finalFogRange, const DirectX::XMVECTOR& startColor, const DirectX::XMVECTOR& finalColor) :
	BaseAnimation(startInMilliseconds, durationInMilliseconds),
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

	m_graphics.SetFogDistanceParameters(fogStart, fogRange);
	m_graphics.SetFogColor(fogColor);
}

#pragma once

namespace GraphicsEngine
{
	enum class RenderLayer
	{
		Opaque,
		Transparent,
		AlphaClipped,
		Terrain,
		SkyDome,
		SkyClouds,
		Grass,
		NormalSpecularMapping,
		OpaqueDynamicReflectors,
		Debug,
		Count
	};
}

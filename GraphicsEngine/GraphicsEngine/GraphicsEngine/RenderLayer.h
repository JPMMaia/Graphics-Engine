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
		NormalMapping,
		NormalSpecularMapping,
		NormalSpecularMappingTransparent,
		OpaqueDynamicReflectors,
		Debug,
		Count
	};
}

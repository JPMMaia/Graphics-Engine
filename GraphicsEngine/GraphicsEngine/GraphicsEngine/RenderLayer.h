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
		Grass,
		NormalSpecularMapping,
		OpaqueDynamicReflectors,
		Debug,
		Count
	};
}

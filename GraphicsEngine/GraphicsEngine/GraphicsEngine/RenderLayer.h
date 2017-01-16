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
		Debug,
		Count
	};
}

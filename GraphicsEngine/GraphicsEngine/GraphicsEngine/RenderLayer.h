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
		Billboard,
		NormalMapping,
		Debug,
		Count
	};
}

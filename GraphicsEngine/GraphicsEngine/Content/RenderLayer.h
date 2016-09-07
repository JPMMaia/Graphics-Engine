#pragma once

namespace GraphicsEngine
{
	enum class RenderLayer
	{
		Opaque,
		Transparent,
		AlphaTested,
		Mirrors,
		Reflected,
		Shadow,
		Count
	};
}

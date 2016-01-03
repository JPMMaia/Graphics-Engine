#pragma once

#include "Material.h"

namespace GraphicsEngine
{
	struct Subset
	{
		uint32_t StartIndex;
		uint32_t IndexCount;
		Material Material;
	};
}
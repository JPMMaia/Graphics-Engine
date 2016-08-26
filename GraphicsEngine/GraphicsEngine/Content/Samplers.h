#pragma once

#include <array>
#include "d3dx12.h"

namespace GraphicsEngine
{
	namespace Samplers
	{
		std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
	}
}

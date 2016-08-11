#pragma once

#include <DirectXMath.h>

namespace GraphicsEngine
{
	namespace VertexTypes
	{
		struct ColorVertexType
		{
			DirectX::XMFLOAT3 Position;
			DirectX::XMFLOAT4 Color;
		};
	}
}

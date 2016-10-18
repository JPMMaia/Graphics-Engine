#pragma once

#include <DirectXMath.h>

namespace GraphicsEngine
{
	namespace MathHelper
	{
		void CalculateTangentBinormal(const DirectX::XMFLOAT3& coord0, const DirectX::XMFLOAT3& coord1, const DirectX::XMFLOAT3& coord2, const DirectX::XMFLOAT2& texCoord0, const DirectX::XMFLOAT2 texCoord1, const DirectX::XMFLOAT2 texCoord2, DirectX::XMFLOAT3& tangent, DirectX::XMFLOAT3& binormal);
	}
}

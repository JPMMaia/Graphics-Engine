#pragma once

#include "MathHelper.h"

#include <DirectXMath.h>
#include <string>

namespace GraphicsEngine
{
	struct Material
	{
		Material();

		std::string Name;
		int MaterialCBIndex = -1;
		int DiffuseSrvHeapIndex = -1;
		int NormalSrvHeapIndex = -1;
		int FramesDirtyCount;

		// Material constant buffer data used for shading:
		DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.25f;
		DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
	};
}

#pragma once

#include "Common/MathHelper.h"
#include "Texture.h"

#include <DirectXMath.h>
#include <string>

namespace GraphicsEngine
{
	struct Material
	{
		std::string Name;
		int MaterialIndex = -1;
		const Texture* DiffuseMap = nullptr;
		const Texture* NormalMap = nullptr;
		const Texture* TangentMap = nullptr;
		const Texture* HeightMap = nullptr;
		const Texture* TiledDiffuseMap = nullptr;
		const Texture* TiledNormalMap = nullptr;
		const Texture* TiledDiffuseMap2 = nullptr;
		const Texture* TiledNormalMap2 = nullptr;
		const Texture* TiledDiffuseMap3 = nullptr;
		const Texture* TiledNormalMap3 = nullptr;
		const Texture* TiledDiffuseMap4 = nullptr;
		const Texture* TiledNormalMap4 = nullptr;

		// Material constant buffer data used for shading:
		DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.25f;
		DirectX::XMFLOAT4X4 MaterialTransform = MathHelper::Identity4x4();
	};
}

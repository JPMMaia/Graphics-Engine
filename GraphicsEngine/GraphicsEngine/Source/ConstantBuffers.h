#pragma once

#include <DirectXMath.h>

#include "DirectionalLight.h"
#include "Material.h"
#include "PointLight.h"
#include "SpotLight.h"

namespace GraphicsEngine
{
	namespace ConstantBuffers
	{
		struct CameraConstantBuffer
		{
			DirectX::XMFLOAT4X4 ViewProjectionMatrix;
			DirectX::XMFLOAT3 EyePositionW;
			float Pad;
		};

		struct TesselationConstantBuffer
		{
			float MaxTesselationDistance;
			float MinTesselationDistance;
			float MaxTesselationFactor;
			float MinTesselationFactor;
		};

		struct SubsetConstantBuffer
		{
			Material Material;
		};

		struct FrameConstantBuffer
		{
			DirectionalLight DirectionalLight;
			PointLight PointLight;
			SpotLight SpotLight;
		};
	}
}

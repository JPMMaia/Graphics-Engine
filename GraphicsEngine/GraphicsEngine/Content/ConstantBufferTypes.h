#pragma once

#include "MathHelper.h"

namespace GraphicsEngine
{
	namespace ConstantBufferTypes
	{
		struct ObjectConstants
		{
			DirectX::XMFLOAT4X4 WorldMatrix = MathHelper::Identity4x4();
		};

		struct PassConstants
		{
			DirectX::XMFLOAT4X4 ViewMatrix = MathHelper::Identity4x4();
			DirectX::XMFLOAT4X4 InverseViewMatrix = MathHelper::Identity4x4();
			DirectX::XMFLOAT4X4 ProjectionMatrix = MathHelper::Identity4x4();
			DirectX::XMFLOAT4X4 InverseProjectionMatrix = MathHelper::Identity4x4();
			DirectX::XMFLOAT4X4 ViewProjectionMatrix = MathHelper::Identity4x4();
			DirectX::XMFLOAT4X4 InverseViewProjectionMatrix = MathHelper::Identity4x4();
			DirectX::XMFLOAT3 EyePositionW;
			float Padding;
			DirectX::XMFLOAT2 RenderTargetSize;
			DirectX::XMFLOAT2 InverseRenderTargetSize;
			float NearZ;
			float FarZ;
			float TotalTime;
			float DeltaTime;
		};
	}
}

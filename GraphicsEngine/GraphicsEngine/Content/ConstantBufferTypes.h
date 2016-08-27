#pragma once

#include "MathHelper.h"
#include "Light.h"

namespace GraphicsEngine
{
	namespace ConstantBufferTypes
	{
		struct ObjectConstants
		{
			DirectX::XMFLOAT4X4 WorldMatrix = MathHelper::Identity4x4();
			DirectX::XMFLOAT4X4 TextureTransform = MathHelper::Identity4x4();
		};

		struct MaterialConstants
		{
			DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };;
			DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
			float Roughness = 0.25f;
			DirectX::XMFLOAT4X4 MaterialTransform = MathHelper::Identity4x4();
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
			DirectX::XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };
			Light Lights[16];
		};
	}
}

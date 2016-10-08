#pragma once

#include "MathHelper.h"
#include "Light.h"

namespace GraphicsEngine
{
	namespace BufferTypes
	{
		struct InstanceData
		{
			DirectX::XMFLOAT4X4 WorldMatrix = MathHelper::Identity4x4();
			DirectX::XMFLOAT4X4 TextureTransform = MathHelper::Identity4x4();
			UINT MaterialIndex = 0;
			UINT Pad0;
			UINT Pad1;
			UINT Pad2;
		};

		struct MaterialData
		{
			DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };;
			DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
			float Roughness = 0.25f;
			DirectX::XMFLOAT4X4 MaterialTransform = MathHelper::Identity4x4();
			UINT DiffuseMapIndex = 0;
			UINT Pad0;
			UINT Pad1;
			UINT Pad2;
		};

		struct PassData
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
			DirectX::XMFLOAT4 FogColor = { 0.5f, 0.5f, 0.5f, 1.0f };
			float FogStart = 5.0f;
			float FogRange = 20.0f;
			DirectX::XMFLOAT2 Padding2;
			DirectX::XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };
			Light Lights[16];
		};
	}
}
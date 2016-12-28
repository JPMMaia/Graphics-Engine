#pragma once

#include "Common/MathHelper.h"

namespace GraphicsEngine
{
	namespace ShaderBufferTypes
	{
		struct InstanceData
		{
			DirectX::XMFLOAT4X4 WorldMatrix = MathHelper::Identity4x4();
		};

		struct MaterialData
		{
			DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };;
			DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
			float Roughness = 0.25f;
			DirectX::XMFLOAT4X4 MaterialTransform = MathHelper::Identity4x4();
		};

		struct LightData
		{
			DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };		// Light color
			float FalloffStart = 1.0f;								// Point/Spot light only
			DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };	// Directional/Spot light only
			float FalloffEnd = 10.0f;								// Point/Spot light only
			DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };		// Point/Spot light only
			float SpotPower = 64.0f;								// Spot light only
		};

		struct PassData
		{
			DirectX::XMFLOAT4X4 ViewMatrix = MathHelper::Identity4x4();
			DirectX::XMFLOAT4X4 InverseViewMatrix = MathHelper::Identity4x4();
			DirectX::XMFLOAT4X4 ProjectionMatrix = MathHelper::Identity4x4();
			DirectX::XMFLOAT4X4 InverseProjectionMatrix = MathHelper::Identity4x4();
			DirectX::XMFLOAT4X4 ViewProjectionMatrix = MathHelper::Identity4x4();
			DirectX::XMFLOAT4X4 InverseViewProjectionMatrix = MathHelper::Identity4x4();
			DirectX::XMFLOAT4X4 ShadowMatrix = MathHelper::Identity4x4();
			DirectX::XMFLOAT3 EyePositionW;
			float TerrainDisplacementScalarY;
			DirectX::XMFLOAT2 RenderTargetSize;
			DirectX::XMFLOAT2 InverseRenderTargetSize;
			float NearZ;
			float FarZ;
			float TotalTime;
			float DeltaTime;
			DirectX::XMFLOAT4 FogColor = { 0.5f, 0.5f, 0.5f, 1.0f };
			float FogStart = 5.0f;
			float FogRange = 20.0f;
			DirectX::XMFLOAT2 Padding1;
			float MaxTesselationDistance = 100.0f;
			float MaxTesselationFactor = 5.0f;
			float MinTesselationDistance = 1000.0f;
			float MinTesselationFactor = 1.0f;
			DirectX::XMFLOAT2 TexelSize;
			float TiledTexelScale;
			float Padding2;
			DirectX::XMFLOAT4 SkyDomeColors[2];
			DirectX::XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

			static constexpr auto MaxNumLights = 16;
			std::array<LightData, MaxNumLights> Lights;
		};
	}
}

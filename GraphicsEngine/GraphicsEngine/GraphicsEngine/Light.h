#pragma once

#include <DirectXMath.h>

namespace GraphicsEngine
{
	struct Light
	{
		DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };		// Light color
		float FalloffStart = 1.0f;								// Point/Spot light only
		DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };	// Directional/Spot light only
		float FalloffEnd = 10.0f;								// Point/Spot light only
		DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };		// Point/Spot light only
		float SpotPower = 64.0f;								// Spot light only
	};
}

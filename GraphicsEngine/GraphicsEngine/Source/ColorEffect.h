#pragma once

#include "BufferTypes.h"
#include "ConstantBuffers.h"
#include "Effect.h"

namespace GraphicsEngine
{
	class ColorEffect : public Effect<EffectTypes::VSEffect, EffectTypes::PSEffect>
	{
	public:
		struct InstanceData
		{
			DirectX::XMFLOAT4X4 WorldMatrix;
		};

	public:
		ColorEffect();
		explicit ColorEffect(ID3D11Device* d3dDevice);

		void Reset();

		void UpdateCameraBuffer(ID3D11DeviceContext1* d3dDeviceContext, const ConstantBuffers::CameraConstantBuffer& buffer) const;

	private:
		static const std::array<D3D11_INPUT_ELEMENT_DESC, 6> s_INPUT_ELEMENT_DESCRIPTION;

		DynamicConstantBuffer m_cameraBuffer;
	};
}

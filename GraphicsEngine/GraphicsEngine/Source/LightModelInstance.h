#pragma once

#include "LightModel.h"

namespace GraphicsEngine
{
	class LightModelInstance
	{
	public:
		LightModelInstance();
		LightModelInstance(const LightModel* model);

		void Initialize(const LightModel* model);

		void Draw(ID3D11DeviceContext1* d3dDeviceContext, LightEffect& lightEffect) const;

		void SetInstanceBuffer(const DirectX::XMFLOAT4X4& worldMatrix, const DirectX::XMFLOAT4X4& viewMatrix, const DirectX::XMFLOAT4X4& projectionMatrix);

	private:
		const LightModel* m_model;
		LightEffect::PerObjectConstantBuffer m_instanceBuffer;
	};
}

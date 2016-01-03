#pragma once

#include "EffectManager.h"
#include "LightModelInstance.h"
#include "TextureManager.h"

namespace GraphicsEngine
{
	class Scene
	{
	public:
		void Initialize(ID3D11Device* d3dDevice);
		void Reset();

		void Render(ID3D11DeviceContext1* d3dDeviceContext);

		void SetModelMatrix(const DirectX::XMFLOAT4X4& modelMatrix);
		void SetViewMatrix(const DirectX::XMFLOAT4X4& viewMatrix);
		void SetProjectionMatrix(const DirectX::XMFLOAT4X4& projectionMatrix);

	private:
		EffectManager m_effectManager;
		LightModel m_cubeModel;
		LightModelInstance m_cubeInstance;
		
		TextureManager m_textureManager;
		LightEffect::PerFrameConstantBuffer m_frameBuffer;

		DirectX::XMFLOAT4X4 m_modelMatrix;
		DirectX::XMFLOAT4X4 m_viewMatrix;
		DirectX::XMFLOAT4X4 m_projectionMatrix;
	};
}

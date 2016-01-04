#pragma once

#include "EffectManager.h"
#include "LightModelInstance.h"
#include "TextureManager.h"
#include "Camera.h"
#include "InputHandler.h"

namespace GraphicsEngine
{
	class Scene
	{
	public:
		void Initialize(ID3D11Device* d3dDevice);
		void Reset();

		void Render(ID3D11DeviceContext1* d3dDeviceContext);

		void SetProjectionMatrix(const DirectX::XMFLOAT4X4& projectionMatrix);

		void HandleInput(const InputHandler& input);

	private:
		EffectManager m_effectManager;
		LightModel m_cubeModel;
		LightModelInstance m_cubeInstance;
		
		TextureManager m_textureManager;
		LightEffect::PerFrameConstantBuffer m_frameBuffer;
		Camera m_camera;

		DirectX::XMFLOAT4X4 m_modelMatrix;
		DirectX::XMFLOAT4X4 m_viewMatrix;
		DirectX::XMFLOAT4X4 m_projectionMatrix;
	};
}

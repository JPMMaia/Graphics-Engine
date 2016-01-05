#pragma once

#include "EffectManager.h"
#include "TextureManager.h"
#include "Camera.h"
#include "InputHandler.h"
#include "LightModel.h"

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
		void UpdateCamera();
		void InitializeCubeInstancedData(ID3D11DeviceContext1* d3dDeviceContext) const;

	private:
		EffectManager m_effectManager;
		LightModel m_cubeModel;
		
		TextureManager m_textureManager;
		LightEffect::FrameConstantBuffer m_frameBuffer;
		LightEffect::CameraConstantBuffer m_cameraBuffer;
		Camera m_camera;

		DirectX::XMFLOAT4X4 m_projectionMatrix;
		bool m_firstRun;
	};
}

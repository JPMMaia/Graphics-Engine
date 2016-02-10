#pragma once

#include "EffectManager.h"
#include "TextureManager.h"
#include "Camera.h"
#include "InputHandler.h"
#include "LightModel.h"
#include "TerrainModel.h"
#include "ConstantBuffers.h"

namespace GraphicsEngine
{
	__declspec(align(16))
	class Scene
	{
	public:
		void* operator new(size_t size);
		void operator delete(void* pointer);

		void CreateDeviceDependentResources(ID3D11Device* d3dDevice);
		void CreateWindowSizeDependentResources(float screenWidth, float screenHeight, const DirectX::XMMATRIX& orientationMatrix);
		void ReleaseDeviceDependentResources();

		void Render(ID3D11DeviceContext1* d3dDeviceContext);

		void HandleInput(const InputHandler& input);

	private:
		void InitializeCubeModel(ID3D11Device* d3dDevice);
		void InitializeFrameBuffer();
		void InitializeTesselationBuffer();
		void UpdateCamera();

	private:
		EffectManager m_effectManager;
		LightModel m_cubeModel;
		TerrainModel m_terrainModel;
		
		TextureManager m_textureManager;
		ConstantBuffers::FrameConstantBuffer m_frameBuffer;
		ConstantBuffers::CameraConstantBuffer m_cameraBuffer;
		ConstantBuffers::TesselationConstantBuffer m_tesselationBuffer;
		Camera m_camera;
	};
}

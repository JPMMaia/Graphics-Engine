#pragma once

#include "LightEffect.h"
#include "Mesh.h"
#include "RasterizerState.h"

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
		LightEffect m_lightEffect;
		Mesh m_cubeMesh;

		LightEffect::PerFrameConstantBuffer m_frameBuffer;
		LightEffect::PerObjectConstantBuffer m_cubeBuffer;

		DirectX::XMFLOAT4X4 m_modelMatrix;
		DirectX::XMFLOAT4X4 m_viewMatrix;
		DirectX::XMFLOAT4X4 m_projectionMatrix;

		RasterizerState m_rasterizerState;
	};
}

#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "DirectXMath.h"
#include "Shaders/Material.h"
#include "ConstantBuffer.h"
#include "Shaders/DirectionalLight.h"

namespace GraphicsEngine
{
	class LightEffect
	{
	public:
		struct PerObjectConstantBuffer
		{
			DirectX::XMFLOAT4X4 WorldMatrix;
			DirectX::XMFLOAT4X4 WorldInverseTransposeMatrix;
			DirectX::XMFLOAT4X4 WorldViewProjectionMatrix;
			Material Material;
			DirectX::XMFLOAT3 EyePositionW;
		};
		struct PerFrameConstantBuffer
		{
			DirectionalLight DirectionalLight;
		};

	public:
		LightEffect();
		LightEffect(ID3D11Device* d3dDevice);
		
		bool Initialize(ID3D11Device* d3dDevice);
		void Shutdown();

		void UpdatePerObjectConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const PerObjectConstantBuffer& buffer);
		void UpdatePerFrameConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const PerFrameConstantBuffer& buffer);

		void Set(ID3D11DeviceContext1* d3dDeviceContext);

	private:
		VertexShader m_vertexShader;
		PixelShader m_pixelShader;
		ConstantBuffer<PerObjectConstantBuffer> m_perObjectConstantBuffer;
		ConstantBuffer<PerFrameConstantBuffer> m_perFrameConstantBuffer;
	};
}

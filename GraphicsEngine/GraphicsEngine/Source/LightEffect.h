#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "DirectXMath.h"
#include "Material.h"
#include "DynamicConstantBuffer.h"
#include "DirectionalLight.h"
#include "Technique.h"

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
			float Pad;
		};
		struct PerFrameConstantBuffer
		{
			DirectionalLight DirectionalLight;
		};

	public:
		LightEffect();
		LightEffect(ID3D11Device* d3dDevice);
		
		void Initialize(ID3D11Device* d3dDevice);
		void Reset();

		void UpdatePerObjectConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const PerObjectConstantBuffer& buffer) const;
		void UpdatePerFrameConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const PerFrameConstantBuffer& buffer) const;

		void Set(ID3D11DeviceContext1* d3dDeviceContext) const;

	private:
		VertexShader m_vertexShader;
		PixelShader m_pixelShader;
		DynamicConstantBuffer m_perObjectConstantBuffer;
		DynamicConstantBuffer m_perFrameConstantBuffer;
		Technique m_lightTechnique;
	};
}
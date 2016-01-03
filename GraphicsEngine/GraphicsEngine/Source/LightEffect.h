#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "DirectXMath.h"
#include "Material.h"
#include "DynamicConstantBuffer.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Technique.h"
#include "Texture.h"

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
		};
		struct PerSubsetConstantBuffer
		{
			Material Material;
		};
		struct PerFrameConstantBuffer
		{
			DirectionalLight DirectionalLight;
			PointLight PointLight;
			SpotLight SpotLight;
			DirectX::XMFLOAT3 EyePositionW;
			float Pad;
		};

	public:
		LightEffect();
		LightEffect(ID3D11Device* d3dDevice);
		
		void Initialize(ID3D11Device* d3dDevice);
		void Reset();

		void UpdatePerObjectConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const PerObjectConstantBuffer& buffer) const;
		void UpdatePerSubsetConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const PerSubsetConstantBuffer& buffer) const;
		void UpdatePerFrameConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const PerFrameConstantBuffer& buffer) const;		

		void Set(ID3D11DeviceContext1* d3dDeviceContext) const;

		static void SetTexture(ID3D11DeviceContext1* d3dDeviceContext, const Texture& texture);

	private:
		VertexShader m_vertexShader;
		PixelShader m_pixelShader;
		DynamicConstantBuffer m_perObjectConstantBuffer;
		DynamicConstantBuffer m_perSubsetConstantBuffer;
		DynamicConstantBuffer m_perFrameConstantBuffer;
		SamplerState m_samplerState;
		Technique m_lightTechnique;
	};
}
#pragma once

#include "InstancedDataTypes.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "Technique.h"
#include "Texture.h"
#include "BufferTypes.h"
#include "ConstantBuffers.h"

namespace GraphicsEngine
{
	class LightEffect
	{
	public:
		using InstanceData = InstancedDataTypes::World;

	public:
		LightEffect();
		explicit LightEffect(ID3D11Device* d3dDevice);
		
		void Initialize(ID3D11Device* d3dDevice);
		void Reset();

		void UpdateCameraConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const ConstantBuffers::CameraConstantBuffer& buffer) const;
		void UpdateTesselationConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const ConstantBuffers::TesselationConstantBuffer& buffer) const;
		void UpdateSubsetConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const ConstantBuffers::SubsetConstantBuffer& buffer) const;
		void UpdateFrameConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const ConstantBuffers::FrameConstantBuffer& buffer) const;

		void Set(ID3D11DeviceContext1* d3dDeviceContext) const;

		static void SetTextureMap(ID3D11DeviceContext1* d3dDeviceContext, const Texture& textureMap);
		static void SetNormalMap(ID3D11DeviceContext1* d3dDeviceContext, const Texture& normalMap);
		static void SetHeightMap(ID3D11DeviceContext1* d3dDeviceContext, const Texture& heightMap);

	private:
		VertexShader m_vertexShader;
		HullShader m_hullShader;
		DomainShader m_domainShader;
		PixelShader m_pixelShader;
		DynamicConstantBuffer m_cameraConstantBuffer;
		DynamicConstantBuffer m_tesselationConstantBuffer;
		DynamicConstantBuffer m_subsetConstantBuffer;
		DynamicConstantBuffer m_frameConstantBuffer;
		SamplerState m_samplerState;
		RasterizerState m_rasterizerState;
		Technique m_lightTechnique;
	};
}
#pragma once

#include "BaseEffect.h"
#include "VertexShader.h"
#include "HullShader.h"
#include "DomainShader.h"
#include "PixelShader.h"
#include "RasterizerState.h"
#include "SamplerState.h"
#include "Texture.h"
#include "BufferTypes.h"
#include "ConstantBuffers.h"
#include "InstancedDataTypes.h"

namespace GraphicsEngine
{
	class LightEffect : public BaseEffect<LightEffect>
	{
	public:
		using InstanceData = InstancedDataTypes::World;
		using CameraBuffer = ConstantBuffers::CameraConstantBuffer;
		using FrameBuffer = ConstantBuffers::FrameConstantBuffer;
		using SubsetBuffer = ConstantBuffers::SubsetConstantBuffer;
		using TesselationBuffer = ConstantBuffers::TesselationConstantBuffer;

	public:
		LightEffect();
		explicit LightEffect(ID3D11Device* d3dDevice);

		void Reset();

		void Set(ID3D11DeviceContext1* d3dDeviceContext) const;

		void UpdateCameraBuffer(ID3D11DeviceContext1* d3dDeviceContext, const CameraBuffer& buffer) const;
		void UpdateFrameBuffer(ID3D11DeviceContext1* d3dDeviceContext, const FrameBuffer& buffer) const;
		void UpdateSubsetBuffer(ID3D11DeviceContext1* d3dDeviceContext, const SubsetBuffer& buffer) const;
		void UpdateTesselationBuffer(ID3D11DeviceContext1* d3dDeviceContext, const TesselationBuffer& buffer) const;

		static void SetTextureMap(ID3D11DeviceContext1* d3dDeviceContext, const Texture& textureMap);
		static void SetNormalMap(ID3D11DeviceContext1* d3dDeviceContext, const Texture& normalMap);
		static void SetHeightMap(ID3D11DeviceContext1* d3dDeviceContext, const Texture& heightMap);

	private:
		static const std::array<D3D11_INPUT_ELEMENT_DESC, 8> s_INPUT_ELEMENT_DESCRIPTION;

		VertexShader m_vertexShader;
		HullShader m_hullShader;
		DomainShader m_domainShader;
		PixelShader m_pixelShader;
		DynamicConstantBuffer m_cameraBuffer;
		DynamicConstantBuffer m_frameBuffer;
		DynamicConstantBuffer m_subsetBuffer;
		DynamicConstantBuffer m_tesselationBuffer;
		SamplerState m_samplerState;
		RasterizerState m_rasterizerState;
	};
}

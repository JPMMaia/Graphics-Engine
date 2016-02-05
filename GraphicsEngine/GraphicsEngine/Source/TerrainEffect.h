#pragma once

#include "BufferTypes.h"
#include "VertexShader.h"
#include "HullShader.h"
#include "DomainShader.h"
#include "PixelShader.h"
#include "RasterizerState.h"
#include "SamplerState.h"
#include "Technique.h"
#include "ConstantBuffers.h"
#include "Texture.h"

namespace GraphicsEngine
{
	class TerrainEffect
	{
	public:
		TerrainEffect();
		TerrainEffect(ID3D11Device* d3dDevice);

		void Initialize(ID3D11Device* d3dDevice);
		void Reset();

		void UpdateCameraConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const ConstantBuffers::CameraConstantBuffer& buffer) const;
		void UpdateTesselationConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const ConstantBuffers::TesselationConstantBuffer& buffer) const;
		void UpdateSubsetConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const ConstantBuffers::SubsetConstantBuffer& buffer) const;
		void UpdateFrameConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const ConstantBuffers::FrameConstantBuffer& buffer) const;

		void Set(ID3D11DeviceContext1* d3dDeviceContext) const;

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
		Technique m_terrainTechnique;
	};
}

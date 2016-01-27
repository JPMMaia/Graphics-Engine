#pragma once

#include <DirectXMath.h>
#include "Material.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "BufferTypes.h"
#include "VertexShader.h"
#include "HullShader.h"
#include "DomainShader.h"
#include "PixelShader.h"
#include "RasterizerState.h"
#include "SamplerState.h"
#include "Technique.h"

namespace GraphicsEngine
{
	class TerrainEffect
	{
	public:
		struct CameraConstantBuffer
		{
			DirectX::XMFLOAT4X4 ViewProjectionMatrix;
			DirectX::XMFLOAT3 EyePositionW;
			float Pad;
		};
		struct TesselationConstantBuffer
		{
			float MaxTesselationDistance;
			float MinTesselationDistance;
			float MaxTesselationFactor;
			float MinTesselationFactor;
		};
		struct SubsetConstantBuffer
		{
			Material Material;
		};
		struct FrameConstantBuffer
		{
			DirectionalLight DirectionalLight;
			PointLight PointLight;
			SpotLight SpotLight;
		};

	public:
		TerrainEffect();
		TerrainEffect(ID3D11Device* d3dDevice);

		void Initialize(ID3D11Device* d3dDevice);
		void Reset();

		void UpdateCameraConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const CameraConstantBuffer& buffer) const;
		void UpdateTesselationConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const TesselationConstantBuffer& buffer) const;
		void UpdateSubsetConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const SubsetConstantBuffer& buffer) const;
		void UpdateFrameConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const FrameConstantBuffer& buffer) const;

		void Set(ID3D11DeviceContext1* d3dDeviceContext) const;

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

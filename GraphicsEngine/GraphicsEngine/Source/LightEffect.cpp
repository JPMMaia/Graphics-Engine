﻿#include "stdafx.h"
#include "LightEffect.h"

#include "SamplerStateDescConstants.h"

using namespace GraphicsEngine;
using namespace std;

LightEffect::LightEffect()
{
}

LightEffect::LightEffect(ID3D11Device* d3dDevice)
{
	Initialize(d3dDevice);
}

void LightEffect::Initialize(ID3D11Device* d3dDevice)
{
	// Describe the input layout:
	static const vector<D3D11_INPUT_ELEMENT_DESC> inputDesc = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	// Initialize the vertex shader:
	m_vertexShader.Initialize(d3dDevice, L"LightVertexShader.cso", inputDesc);

	// Initialize the pixel shader:
	m_pixelShader.Initialize(d3dDevice, L"LightPixelShader.cso");

	// Initialize constant buffers:
	m_cameraConstantBuffer.Initialize(d3dDevice, sizeof(CameraConstantBuffer));
	m_subsetConstantBuffer.Initialize(d3dDevice, sizeof(SubsetConstantBuffer));
	m_frameConstantBuffer.Initialize(d3dDevice, sizeof(FrameConstantBuffer));

	// Initialize the sampler state:
	m_samplerState.Initialize(d3dDevice, SamplerStateDescConstants::Anisotropic);

	// Setup light technique:
	m_lightTechnique.SetVertexShader(&m_vertexShader);
	m_lightTechnique.SetPixelShader(&m_pixelShader);
	m_lightTechnique.VSSetConstantBuffer(m_cameraConstantBuffer, 0);
	m_lightTechnique.PSSetConstantBuffer(m_subsetConstantBuffer, 1);
	m_lightTechnique.PSSetConstantBuffer(m_frameConstantBuffer, 2);
	m_lightTechnique.PSSetSamplerState(m_samplerState, 0);
}

void LightEffect::Reset()
{
	m_samplerState.Reset();
	m_frameConstantBuffer.Reset();
	m_subsetConstantBuffer.Reset();
	m_cameraConstantBuffer.Reset();
	m_pixelShader.Reset();
	m_vertexShader.Reset();
}

void LightEffect::SetTexture(ID3D11DeviceContext1* d3dDeviceContext, const Texture& texture)
{
	d3dDeviceContext->PSSetShaderResources(0, 1, texture.GetAddressOf());
}

void LightEffect::UpdateCameraConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const CameraConstantBuffer& buffer) const
{
	m_cameraConstantBuffer.Update(d3dDeviceContext, &buffer);
}

void LightEffect::UpdateSubsetConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const SubsetConstantBuffer& buffer) const
{
	m_subsetConstantBuffer.Update(d3dDeviceContext, &buffer);
}

void LightEffect::UpdateFrameConstantBuffer(ID3D11DeviceContext1* d3dDeviceContext, const FrameConstantBuffer& buffer) const
{
	m_frameConstantBuffer.Update(d3dDeviceContext, &buffer);
}

void LightEffect::Set(ID3D11DeviceContext1* d3dDeviceContext) const
{
	// Set technique:
	m_lightTechnique.Set(d3dDeviceContext);
}

#include "stdafx.h"
#include "ColorEffect.h"

using namespace GraphicsEngine;

const std::array<D3D11_INPUT_ELEMENT_DESC, 6> ColorEffect::s_INPUT_ELEMENT_DESCRIPTION =
{
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	}
};

ColorEffect::ColorEffect(ID3D11Device* d3dDevice) :
	Effect(
		EffectTypes::VSEffect(d3dDevice, L"ColorVertexShader.cso", s_INPUT_ELEMENT_DESCRIPTION),
		EffectTypes::PSEffect(d3dDevice, L"ColorPixelShader.cso"),
		Technique2(
			nullptr,
			{ 
				{ &ID3D11DeviceContext1::VSSetConstantBuffers, { m_cameraBuffer.Get() } }
			},
			{}
			)
		)
{
}

void ColorEffect::UpdateCameraBuffer(ID3D11DeviceContext1* d3dDeviceContext, const ConstantBuffers::CameraConstantBuffer& buffer) const
{
	m_cameraBuffer.Map(d3dDeviceContext, &buffer, sizeof(ConstantBuffers::CameraConstantBuffer));
}
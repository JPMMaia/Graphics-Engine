#include "stdafx.h"
#include "ColorEffect.h"

#include <array>

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

ColorEffect::ColorEffect()
{
}
ColorEffect::ColorEffect(ID3D11Device* d3dDevice) :
	m_vertexShader(d3dDevice, L"ColorVertexShader.cso", s_INPUT_ELEMENT_DESCRIPTION),
	m_pixelShader(d3dDevice, L"ColorPixelShader.cso"),
	m_cameraBuffer(d3dDevice, sizeof(CameraBuffer))
{
}

void ColorEffect::Reset()
{
	m_cameraBuffer.Reset();
}

void ColorEffect::Set(ID3D11DeviceContext1* d3dDeviceContext) const
{
	// Set vertex and pixel shaders:
	m_vertexShader.Set(d3dDeviceContext);
	d3dDeviceContext->HSSetShader(nullptr, nullptr, 0);
	d3dDeviceContext->DSSetShader(nullptr, nullptr, 0);
	m_pixelShader.Set(d3dDeviceContext);

	// Set constant buffers:
	d3dDeviceContext->VSSetConstantBuffers(0, 1, m_cameraBuffer.GetAddressOf());
}

void ColorEffect::UpdateCameraBuffer(ID3D11DeviceContext1* d3dDeviceContext, const CameraBuffer& buffer) const
{
	m_cameraBuffer.Map(d3dDeviceContext, &buffer, sizeof(CameraBuffer));
}

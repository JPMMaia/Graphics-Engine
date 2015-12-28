#include "stdafx.h"
#include "VertexShader.h"

using namespace GraphicsEngine;
using namespace std;

VertexShader::VertexShader()
{
}

VertexShader::VertexShader(ID3D11Device* d3dDevice, const std::wstring& filename, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDesc)
{
	Initialize(d3dDevice, filename, vertexDesc);
}

void VertexShader::Initialize(ID3D11Device* d3dDevice, const std::wstring& filename, const vector<D3D11_INPUT_ELEMENT_DESC>& vertexDesc)
{
	// Read data from vertex shader file:
	vector<char> fileData;
	Helpers::ReadData(filename, fileData);

	// Create vertex shader:
	DX::ThrowIfFailed(
		d3dDevice->CreateVertexShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			m_vertexShader.GetAddressOf()
			)
		);

	// Create input layout:
	DX::ThrowIfFailed(
		d3dDevice->CreateInputLayout(
			vertexDesc.data(),
			vertexDesc.size(),
			&fileData[0],
			fileData.size(),
			m_inputLayout.GetAddressOf()
			)
		);
}

void VertexShader::Reset()
{
	m_inputLayout.Reset();
	m_vertexShader.Reset();
}

void VertexShader::Set(ID3D11DeviceContext* d3dDeviceContext) const
{
	// Set input layout:
	d3dDeviceContext->IASetInputLayout(m_inputLayout.Get());

	// Set vertex shader:
	d3dDeviceContext->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
		);
}

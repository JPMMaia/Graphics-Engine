#include "stdafx.h"
#include "VertexShader.h"

using namespace Common;
using namespace GraphicsEngine;
using namespace std;

VertexShader::VertexShader(ID3D11Device* d3dDevice, const std::wstring& filename, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDesc)
{
	// Read data from vertex shader file:
	vector<char> fileData;
	Helpers::ReadData(filename, fileData);
	Initialize(d3dDevice, &fileData[0], fileData.size(), vertexDesc);
}
VertexShader::VertexShader(ID3D11Device* d3dDevice, const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDesc)
{
	auto blob = IShader::CompileFromFile(filename, defines, entrypoint, target);
	Initialize(d3dDevice, blob->GetBufferPointer(), blob->GetBufferSize(), vertexDesc);
}

void VertexShader::Initialize(ID3D11Device* d3dDevice, const void* shaderByteCode, SIZE_T byteCodeLength, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDesc)
{
	// Create vertex shader:
	Common::ThrowIfFailed(
		d3dDevice->CreateVertexShader(
			shaderByteCode,
			byteCodeLength,
			nullptr,
			m_vertexShader.GetAddressOf()
		)
	);

	// Create input layout:
	Common::ThrowIfFailed(
		d3dDevice->CreateInputLayout(
			vertexDesc.data(),
			static_cast<UINT>(vertexDesc.size()),
			shaderByteCode,
			byteCodeLength,
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

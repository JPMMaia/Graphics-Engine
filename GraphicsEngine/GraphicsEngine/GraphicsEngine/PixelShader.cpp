#include "stdafx.h"
#include "PixelShader.h"

using namespace Common;
using namespace GraphicsEngine;
using namespace std;

PixelShader::PixelShader(ID3D11Device* d3dDevice, const std::wstring& filename)
{
	// Read data from pixel shader file:
	vector<char> fileData;
	Helpers::ReadData(filename, fileData);
	Initialize(d3dDevice, &fileData[0], fileData.size());
}
PixelShader::PixelShader(ID3D11Device* d3dDevice, const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
{
	auto blob = IShader::CompileFromFile(filename, defines, entrypoint, target);
	Initialize(d3dDevice, blob->GetBufferPointer(), blob->GetBufferSize());
}

void PixelShader::Initialize(ID3D11Device* d3dDevice, const void* shaderByteCode, SIZE_T byteCodeLength)
{
	// Create pixel shader:
	ThrowIfFailed(
		d3dDevice->CreatePixelShader(
			shaderByteCode,
			byteCodeLength,
			nullptr,
			m_pixelShader.GetAddressOf()
			)
		);
}
void PixelShader::Reset()
{
	m_pixelShader.Reset();
}

void PixelShader::Set(ID3D11DeviceContext* d3dDeviceContext) const
{
	// Set pixel shader:
	d3dDeviceContext->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
		);
}

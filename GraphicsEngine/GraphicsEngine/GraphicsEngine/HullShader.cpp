#include "stdafx.h"
#include "HullShader.h"

using namespace Common;
using namespace GraphicsEngine;
using namespace std;

const HullShader HullShader::s_null = HullShader();

HullShader::HullShader(ID3D11Device* d3dDevice, const std::wstring& filename)
{
	// Read data from domain shader file:
	vector<char> fileData;
	Helpers::ReadData(filename, fileData);
	Initialize(d3dDevice, &fileData[0], fileData.size());
}
HullShader::HullShader(ID3D11Device* d3dDevice, const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
{
	auto blob = IShader::CompileFromFile(filename, defines, entrypoint, target);
	Initialize(d3dDevice, blob->GetBufferPointer(), blob->GetBufferSize());
}

void HullShader::Initialize(ID3D11Device* d3dDevice, const void* shaderByteCode, SIZE_T byteCodeLength)
{
	// Create domain shader:
	ThrowIfFailed(
		d3dDevice->CreateHullShader(
			shaderByteCode,
			byteCodeLength,
			nullptr,
			m_hullShader.GetAddressOf()
		)
	);
}
void HullShader::Reset()
{
	m_hullShader.Reset();
}

void HullShader::Set(ID3D11DeviceContext* d3dDeviceContext) const
{
	d3dDeviceContext->HSSetShader(m_hullShader.Get(), nullptr, 0);
}
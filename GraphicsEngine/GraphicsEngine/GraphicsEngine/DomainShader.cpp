#include "stdafx.h"
#include "DomainShader.h"

using namespace Common;
using namespace GraphicsEngine;
using namespace std;

DomainShader::DomainShader(ID3D11Device* d3dDevice, const std::wstring& filename)
{
	// Read data from domain shader file:
	vector<char> fileData;
	Helpers::ReadData(filename, fileData);
	Initialize(d3dDevice, &fileData[0], fileData.size());
}
DomainShader::DomainShader(ID3D11Device* d3dDevice, const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
{
	auto blob = IShader::CompileFromFile(filename, defines, entrypoint, target);
	Initialize(d3dDevice, blob->GetBufferPointer(), blob->GetBufferSize());
}

void DomainShader::Initialize(ID3D11Device* d3dDevice, const void* shaderByteCode, SIZE_T byteCodeLength)
{
	// Create domain shader:
	ThrowIfFailed(
		d3dDevice->CreateDomainShader(
			shaderByteCode,
			byteCodeLength,
			nullptr,
			m_domainShader.GetAddressOf()
		)
	);
}
void DomainShader::Reset()
{
	m_domainShader.Reset();
}

void DomainShader::Set(ID3D11DeviceContext* d3dDeviceContext) const
{
	d3dDeviceContext->DSSetShader(m_domainShader.Get(), nullptr, 0);
}
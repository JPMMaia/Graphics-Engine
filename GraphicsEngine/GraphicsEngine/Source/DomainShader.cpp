#include "stdafx.h"
#include "DomainShader.h"

using namespace GraphicsEngine;
using namespace std;

DomainShader::DomainShader()
{
}
DomainShader::DomainShader(ID3D11Device* d3dDevice, const std::wstring& filename)
{
	Initialize(d3dDevice, filename);
}

void DomainShader::Initialize(ID3D11Device* d3dDevice, const std::wstring& filename)
{
	// Read data from domain shader file:
	vector<char> fileData;
	Helpers::ReadData(filename, fileData);

	// Create domain shader:
	DX::ThrowIfFailed(
		d3dDevice->CreateDomainShader(
			&fileData[0],
			fileData.size(),
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
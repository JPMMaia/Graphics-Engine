#include "stdafx.h"
#include "PixelShader.h"

using namespace GraphicsEngine;
using namespace std;

PixelShader::PixelShader()
{
}
PixelShader::PixelShader(ID3D11Device* d3dDevice, const std::wstring& filename)
{
	Initialize(d3dDevice, filename);
}

bool PixelShader::Initialize(ID3D11Device* d3dDevice, const std::wstring& filename)
{
	// Read data from pixel shader file:
	vector<char> fileData;
	if (!Helpers::ReadData(filename, fileData))
		return false;

	// Create pixel shader:
	DX::ThrowIfFailed(
		d3dDevice->CreatePixelShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			m_pixelShader.GetAddressOf()
			)
		);

	return true;
}

void PixelShader::Shutdown()
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

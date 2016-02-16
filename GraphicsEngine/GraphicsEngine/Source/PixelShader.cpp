#include "stdafx.h"
#include "PixelShader.h"

using namespace GraphicsEngine;
using namespace std;

PixelShader::PixelShader()
{
}
PixelShader::PixelShader(ID3D11Device* d3dDevice, const std::wstring& filename)
{
	// Read data from pixel shader file:
	vector<char> fileData;
	Helpers::ReadData(filename, fileData);

	// Create pixel shader:
	DX::ThrowIfFailed(
		d3dDevice->CreatePixelShader(
			&fileData[0],
			fileData.size(),
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

#include "stdafx.h"
#include "HullShader.h"

using namespace GraphicsEngine;
using namespace std;

HullShader::HullShader()
{
}
HullShader::HullShader(ID3D11Device* d3dDevice, const std::wstring& filename)
{
	// Read data from domain shader file:
	vector<char> fileData;
	Helpers::ReadData(filename, fileData);

	// Create domain shader:
	DX::ThrowIfFailed(
		d3dDevice->CreateHullShader(
			&fileData[0],
			fileData.size(),
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
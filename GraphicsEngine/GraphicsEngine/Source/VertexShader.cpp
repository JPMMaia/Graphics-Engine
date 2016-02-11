#include "stdafx.h"
#include "VertexShader.h"

using namespace GraphicsEngine;
using namespace std;

VertexShader::VertexShader()
{
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

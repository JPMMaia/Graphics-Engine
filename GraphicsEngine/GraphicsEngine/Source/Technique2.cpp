#include "stdafx.h"
#include "Technique2.h"

using namespace GraphicsEngine;

constexpr VSTechnique::VSTechnique(const VertexShader* vertexShader) noexcept :
	m_vertexShader(vertexShader)
{
}

constexpr PSTechnique::PSTechnique(const PixelShader* pixelShader) noexcept :
	m_pixelShader(pixelShader)
{
}

void VSTechnique::Set(ID3D11DeviceContext* d3dDeviceContext) const
{
	m_vertexShader->Set(d3dDeviceContext);
}

void PSTechnique::Set(ID3D11DeviceContext* d3dDeviceContext) const
{
	m_pixelShader->Set(d3dDeviceContext);
}

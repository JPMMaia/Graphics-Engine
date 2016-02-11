#include "stdafx.h"
#include "Technique2.h"

using namespace GraphicsEngine;

void VSTechnique::Set(ID3D11DeviceContext* d3dDeviceContext) const
{
	if(d3dDeviceContext != nullptr)
		m_vertexShader->Set(d3dDeviceContext);
}

void PSTechnique::Set(ID3D11DeviceContext* d3dDeviceContext) const
{
	if (d3dDeviceContext != nullptr)
		m_pixelShader->Set(d3dDeviceContext);
}

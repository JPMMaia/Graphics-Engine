#include "stdafx.h"
#include "DefaultConstantBuffer.h"

using namespace GraphicsEngine;

DefaultConstantBuffer::DefaultConstantBuffer()
{
}

DefaultConstantBuffer::DefaultConstantBuffer(ID3D11Device* d3dDevice, uint32_t bufferSize) :
	ConstantBuffer(d3dDevice, bufferSize, D3D11_USAGE_DEFAULT, 0)
{
}

void DefaultConstantBuffer::Initialize(ID3D11Device* d3dDevice, uint32_t bufferSize)
{
	ConstantBuffer::Initialize(d3dDevice, bufferSize, D3D11_USAGE_DEFAULT, 0);
}

void DefaultConstantBuffer::Update(ID3D11DeviceContext1* d3dDeviceContext, const void* constantBufferData) const
{
	// Prepare the constant buffer to send it to the graphics device:
	d3dDeviceContext->UpdateSubresource1(
		ConstantBuffer::Get(),
		0,
		nullptr,
		&constantBufferData,
		0,
		0,
		0
		);
}

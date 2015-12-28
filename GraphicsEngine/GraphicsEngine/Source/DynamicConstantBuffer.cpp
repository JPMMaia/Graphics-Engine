#include "stdafx.h"
#include "DynamicConstantBuffer.h"

using namespace GraphicsEngine;

DynamicConstantBuffer::DynamicConstantBuffer()
{
}

DynamicConstantBuffer::DynamicConstantBuffer(ID3D11Device* d3dDevice, uint32_t bufferSize) : 
	ConstantBuffer(d3dDevice, bufferSize, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE)
{
}

void DynamicConstantBuffer::Initialize(ID3D11Device* d3dDevice, uint32_t bufferSize)
{
	ConstantBuffer::Initialize(d3dDevice, bufferSize, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

void DynamicConstantBuffer::Update(ID3D11DeviceContext1* d3dDeviceContext, const void* constantBufferData) const
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	auto buffer = ConstantBuffer::Get();

	// Disable GPU access to the constant buffer data:
	d3dDeviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// Update the constant buffer:
	auto bufferSize = ConstantBuffer::GetBufferSize();
	memcpy_s(mappedResource.pData, bufferSize, constantBufferData, bufferSize);

	// Reenable GPU access to the constant buffer data:
	d3dDeviceContext->Unmap(buffer, 0);
}

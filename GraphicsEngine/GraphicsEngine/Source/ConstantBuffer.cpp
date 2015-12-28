#include "stdafx.h"
#include "ConstantBuffer.h"

using namespace GraphicsEngine;

ConstantBuffer::ConstantBuffer() :
	m_bufferSize(0)
{
}

ConstantBuffer::ConstantBuffer(ID3D11Device* d3dDevice, uint32_t bufferSize, D3D11_USAGE usage, uint32_t cpuAccessFlags) :
	m_constantBuffer(d3dDevice, bufferSize, D3D11_BIND_CONSTANT_BUFFER, usage, cpuAccessFlags),
	m_bufferSize(bufferSize)
{
}
ConstantBuffer::~ConstantBuffer()
{
}

void ConstantBuffer::Reset()
{
	m_constantBuffer.Reset();
}

void ConstantBuffer::VSSet(ID3D11DeviceContext1* d3dDeviceContext, uint32_t slot)
{
	// Send the constant buffer to the graphics device:
	d3dDeviceContext->VSSetConstantBuffers1(
		slot,
		1,
		m_constantBuffer.GetAddressOf(),
		nullptr,
		nullptr
		);
}

void ConstantBuffer::PSSet(ID3D11DeviceContext1* d3dDeviceContext, uint32_t slot)
{
	// Send the constant buffer to the graphics device:
	d3dDeviceContext->PSSetConstantBuffers1(
		slot,
		1,
		m_constantBuffer.GetAddressOf(),
		nullptr,
		nullptr
		);
}

ID3D11Buffer* ConstantBuffer::Get() const
{
	return m_constantBuffer.Get();
}

ID3D11Buffer** ConstantBuffer::GetAddressOf()
{
	return m_constantBuffer.GetAddressOf();
}

uint32_t ConstantBuffer::GetBufferSize() const
{
	return m_bufferSize;
}

void ConstantBuffer::Initialize(ID3D11Device* d3dDevice, uint32_t bufferSize, D3D11_USAGE usage, uint32_t cpuAccessFlags)
{
	m_constantBuffer.Initialize(d3dDevice, bufferSize, D3D11_BIND_CONSTANT_BUFFER, usage, cpuAccessFlags);
	m_bufferSize = bufferSize;
}
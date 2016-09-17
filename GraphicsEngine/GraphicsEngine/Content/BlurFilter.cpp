#include "stdafx.h"
#include "BlurFilter.h"

#include "d3dx12.h"

using namespace GraphicsEngine;

BlurFilter::BlurFilter(ID3D12Device* d3dDevice, uint32_t width, uint32_t height, DXGI_FORMAT format, CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescriptor, UINT descriptorIncrementSize) :
	m_blur0(d3dDevice, width, height, format, cpuDescriptor, descriptorIncrementSize),
	m_blur1(d3dDevice, width, height, format, cpuDescriptor.Offset(2, descriptorIncrementSize), descriptorIncrementSize)
{
}

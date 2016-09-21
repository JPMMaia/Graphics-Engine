#include "stdafx.h"
#include "RWTexture.h"

using namespace GraphicsEngine;

RWTexture::RWTexture(ID3D12Device* device, UINT64 width, UINT64 height, DXGI_FORMAT format, CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescriptor, UINT descriptorIncrementSize) :
	Width(width),
	Height(height),
	Format(format)
{
	BuildResource(device, width, height, format);
	BuildDescriptors(device, cpuDescriptor, descriptorIncrementSize);
}

void RWTexture::BuildResource(ID3D12Device* device, UINT64 width, UINT64 height, DXGI_FORMAT format)
{
	auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	D3D12_RESOURCE_DESC resourceDescription;
	resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDescription.Alignment = 0;
	resourceDescription.Width = width;
	resourceDescription.Height = static_cast<UINT>(height);
	resourceDescription.DepthOrArraySize = 1;
	resourceDescription.MipLevels = 1;
	resourceDescription.Format = format;
	resourceDescription.SampleDesc.Count = 1;
	resourceDescription.SampleDesc.Quality = 0;
	resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDescription.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDescription,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(this->Resource.GetAddressOf())
		);
}

void RWTexture::BuildDescriptors(ID3D12Device* device, CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescriptor, UINT descriptorIncrementSize) const
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDescription = {};
	srvDescription.Format = this->Format;
	srvDescription.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDescription.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDescription.Texture2D.MostDetailedMip = 0;
	srvDescription.Texture2D.MipLevels = 1;

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDescription = {};
	uavDescription.Format = this->Format;
	uavDescription.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDescription.Texture2D.MipSlice = 0;

	auto destinationDescriptor(cpuDescriptor);
	device->CreateShaderResourceView(this->Resource.Get(), &srvDescription, destinationDescriptor);

	cpuDescriptor.Offset(1, descriptorIncrementSize);
	device->CreateUnorderedAccessView(this->Resource.Get(), nullptr, &uavDescription, destinationDescriptor);
}

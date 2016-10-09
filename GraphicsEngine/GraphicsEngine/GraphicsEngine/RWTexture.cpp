#include "stdafx.h"
#include "RWTexture.h"
#include "DescriptorHeap.h"
#include "D3DBase.h"

using namespace GraphicsEngine;

RWTexture::RWTexture(const D3DBase& d3dBase, DescriptorHeap* pDescriptorHeap, UINT64 width, UINT64 height, DXGI_FORMAT format) :
	Width(width),
	Height(height),
	Format(format)
{
	BuildResource(d3dBase, width, height, format);
	BuildDescriptors(d3dBase, pDescriptorHeap);
}

void RWTexture::BuildResource(const D3DBase& d3dBase, UINT64 width, UINT64 height, DXGI_FORMAT format)
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

	d3dBase.GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDescription,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(this->Resource.GetAddressOf())
		);
}

void RWTexture::BuildDescriptors(const D3DBase& d3dBase, DescriptorHeap* pDescriptorHeap)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDescription = {};
	srvDescription.Format = this->Format;
	srvDescription.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDescription.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDescription.Texture2D.MostDetailedMip = 0;
	srvDescription.Texture2D.MipLevels = 1;

	auto srvHeapIndex = pDescriptorHeap->CreateShaderResourceView(d3dBase, this->Resource.Get(), &srvDescription);
	this->SrvCpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(pDescriptorHeap->Get()->GetCPUDescriptorHandleForHeapStart(), srvHeapIndex, d3dBase.GetCbvSrvUavDescriptorSize());
	this->SrvGpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(pDescriptorHeap->Get()->GetGPUDescriptorHandleForHeapStart(), srvHeapIndex, d3dBase.GetCbvSrvUavDescriptorSize());

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDescription = {};
	uavDescription.Format = this->Format;
	uavDescription.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDescription.Texture2D.MipSlice = 0;

	auto uavHeapIndex = pDescriptorHeap->CreateUnorderedAccessView(d3dBase, this->Resource.Get(), nullptr, &uavDescription);
	this->UavCpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(pDescriptorHeap->Get()->GetCPUDescriptorHandleForHeapStart(), uavHeapIndex, d3dBase.GetCbvSrvUavDescriptorSize());
	this->UavGpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(pDescriptorHeap->Get()->GetGPUDescriptorHandleForHeapStart(), uavHeapIndex, d3dBase.GetCbvSrvUavDescriptorSize());
}

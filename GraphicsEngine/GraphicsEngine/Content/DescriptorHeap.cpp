#include "stdafx.h"
#include "DescriptorHeap.h"
#include "D3DBase.h"
#include "Texture.h"

using namespace GraphicsEngine;

DescriptorHeap::DescriptorHeap(const D3DBase& d3dBase, size_t capacity) :
	m_capacity(capacity),
	m_count(0)
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDescription = {};
	descriptorHeapDescription.NumDescriptors = static_cast<UINT>(capacity);
	descriptorHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDescription.NodeMask = 0;

	auto device = d3dBase.GetDevice();
	DX::ThrowIfFailed(device->CreateDescriptorHeap(&descriptorHeapDescription, IID_PPV_ARGS(m_descriptorHeap.GetAddressOf())));
}

INT DescriptorHeap::CreateShaderResourceView(const D3DBase& d3dBase, const Texture& texture)
{
	assert(m_count < m_capacity && "Descriptor heap is full!");

	D3D12_SHADER_RESOURCE_VIEW_DESC textureViewDescription = {};
	textureViewDescription.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	textureViewDescription.Format = texture.Resource->GetDesc().Format;
	textureViewDescription.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
	textureViewDescription.Texture2D.MipLevels = texture.Resource->GetDesc().MipLevels;
	textureViewDescription.Texture2D.MostDetailedMip = 0;
	textureViewDescription.Texture2D.ResourceMinLODClamp = 0.0f;

	auto heapIndex = static_cast<INT>(m_count++);
	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), heapIndex, d3dBase.GetCbvSrvUavDescriptorSize());
	d3dBase.GetDevice()->CreateShaderResourceView(texture.Resource.Get(), &textureViewDescription, descriptorHandle);

	return heapIndex;
}

ID3D12DescriptorHeap* DescriptorHeap::Get() const
{
	return m_descriptorHeap.Get();
}

size_t DescriptorHeap::GetDescriptorCount() const
{
	return m_count;
}

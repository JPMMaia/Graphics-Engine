#include "stdafx.h"
#include "DescriptorHeap.h"
#include "D3DBase.h"

using namespace Common;
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
	ThrowIfFailed(device->CreateDescriptorHeap(&descriptorHeapDescription, IID_PPV_ARGS(m_descriptorHeap.GetAddressOf())));
}

INT DescriptorHeap::CreateShaderResourceView(const D3DBase& d3dBase, ID3D12Resource* pResource, const D3D12_SHADER_RESOURCE_VIEW_DESC* pDescription)
{
	assert(m_count < m_capacity && "Descriptor heap is full!");
	
	auto heapIndex = static_cast<INT>(m_count++);
	CD3DX12_CPU_DESCRIPTOR_HANDLE destinationDescriptor(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), heapIndex, d3dBase.GetCbvSrvUavDescriptorSize());

	d3dBase.GetDevice()->CreateShaderResourceView(pResource, pDescription, destinationDescriptor);

	return heapIndex;
}

INT DescriptorHeap::CreateUnorderedAccessView(const D3DBase& d3dBase, ID3D12Resource* pResource, ID3D12Resource *pCounterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* pDescription)
{
	assert(m_count < m_capacity && "Descriptor heap is full!");

	auto heapIndex = static_cast<INT>(m_count++);
	CD3DX12_CPU_DESCRIPTOR_HANDLE destinationDescriptor(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), heapIndex, d3dBase.GetCbvSrvUavDescriptorSize());

	d3dBase.GetDevice()->CreateUnorderedAccessView(pResource, pCounterResource, pDescription, destinationDescriptor);

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

#include "stdafx.h"
#include "BlurFilter.h"

#include <array>

using namespace GraphicsEngine;

BlurFilter::BlurFilter(const D3DBase& d3dBase, DescriptorHeap* pDescriptorHeap, uint32_t width, uint32_t height, DXGI_FORMAT format) :
	m_blur0(d3dBase, pDescriptorHeap, width, height, format),
	m_blur1(d3dBase, pDescriptorHeap, width, height, format)
{
}

void BlurFilter::Execute(ID3D12GraphicsCommandList* pCommandList, ID3D12RootSignature* pRootSignature, ID3D12Resource* pInput, ID3D12PipelineState* horizontalBlurPSO) const
{
	// Set compute root signature:
	pCommandList->SetComputeRootSignature(pRootSignature);

	// Copy data from input to blur0:
	std::array<CD3DX12_RESOURCE_BARRIER, 2> barriers0 =
	{
		CD3DX12_RESOURCE_BARRIER::Transition(m_blur0.Resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST),
		CD3DX12_RESOURCE_BARRIER::Transition(pInput, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE)
	};
	pCommandList->ResourceBarrier(static_cast<UINT>(barriers0.size()), barriers0.data());
	pCommandList->CopyResource(m_blur0.Resource.Get(), pInput);

	// Change states of input, blur0 and blur1:
	std::array<CD3DX12_RESOURCE_BARRIER, 2> barriers1 =
	{
		CD3DX12_RESOURCE_BARRIER::Transition(m_blur0.Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ),
		CD3DX12_RESOURCE_BARRIER::Transition(m_blur1.Resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	};
	pCommandList->ResourceBarrier(static_cast<UINT>(barriers1.size()), barriers1.data());

	pCommandList->SetComputeRootDescriptorTable(0, m_blur0.SrvGpuDescriptorHandle);
	pCommandList->SetComputeRootDescriptorTable(1, m_blur1.UavGpuDescriptorHandle);

	pCommandList->SetPipelineState(horizontalBlurPSO);

	auto groupXCount = static_cast<UINT>(ceilf(m_blur0.Width / 256.0f));
	pCommandList->Dispatch(groupXCount, static_cast<UINT>(m_blur0.Height), 1);

	// Copy result back to input:
	std::array<CD3DX12_RESOURCE_BARRIER, 2> barriers2 =
	{
		CD3DX12_RESOURCE_BARRIER::Transition(m_blur1.Resource.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE),
		CD3DX12_RESOURCE_BARRIER::Transition(pInput, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST)
	};
	pCommandList->ResourceBarrier(static_cast<UINT>(barriers2.size()), barriers2.data());
	pCommandList->CopyResource(pInput, m_blur1.Resource.Get());

	// Restore initial states:
	std::array<CD3DX12_RESOURCE_BARRIER, 3> barriers3 =
	{
		CD3DX12_RESOURCE_BARRIER::Transition(m_blur0.Resource.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COMMON),
		CD3DX12_RESOURCE_BARRIER::Transition(m_blur1.Resource.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COMMON),
		CD3DX12_RESOURCE_BARRIER::Transition(pInput, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET)
	};
	pCommandList->ResourceBarrier(static_cast<UINT>(barriers3.size()), barriers3.data());
}

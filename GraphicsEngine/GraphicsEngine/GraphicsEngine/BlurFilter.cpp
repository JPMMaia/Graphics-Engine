#include "stdafx.h"
#include "BlurFilter.h"

#include <array>

using namespace GraphicsEngine;

BlurFilter::BlurFilter(const D3DBase& d3dBase, DescriptorHeap* pDescriptorHeap, uint32_t width, uint32_t height, DXGI_FORMAT format) :
	m_blur0(d3dBase, pDescriptorHeap, width, height, format),
	m_blur1(d3dBase, pDescriptorHeap, width, height, format)
{
}

void BlurFilter::Execute(ID3D12GraphicsCommandList* pCommandList, ID3D12RootSignature* pRootSignature, ID3D12Resource* pInput, ID3D12PipelineState* horizontalBlurPSO, ID3D12PipelineState* verticalBlurPSO, size_t blurCount) const
{
	// Set compute root signature:
	pCommandList->SetComputeRootSignature(pRootSignature);

	// Copy data from input to blur0:
	{
		std::array<CD3DX12_RESOURCE_BARRIER, 2> barriers =
		{
			CD3DX12_RESOURCE_BARRIER::Transition(m_blur0.Resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST),
			CD3DX12_RESOURCE_BARRIER::Transition(pInput, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE)
		};
		pCommandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
		pCommandList->CopyResource(m_blur0.Resource.Get(), pInput);
	}

	// Change states of blur0 to Generic Read and blur1 to Unordered Access:
	{
		std::array<CD3DX12_RESOURCE_BARRIER, 2> barriers =
		{
			CD3DX12_RESOURCE_BARRIER::Transition(m_blur0.Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ),
			CD3DX12_RESOURCE_BARRIER::Transition(m_blur1.Resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
		};
		pCommandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
	}

	constexpr auto sigma = 2.5f;
	auto weights = CalculateGaussianWeights(sigma);
	pCommandList->SetComputeRoot32BitConstant(0, static_cast<UINT>(floorf(weights.size() / 2.0f)), 0);
	pCommandList->SetComputeRoot32BitConstants(0, static_cast<UINT>(weights.size() - 1), weights.data(), 1);
	
	auto groupXCount = static_cast<UINT>(ceilf(m_blur0.Width / 256.0f));
	auto groupYCount = static_cast<UINT>(ceilf(m_blur0.Height / 256.0f));

	for(size_t i = 0; i < blurCount; ++i)
	{
		pCommandList->SetPipelineState(horizontalBlurPSO);
		pCommandList->SetComputeRootDescriptorTable(1, m_blur0.SrvGpuDescriptorHandle);
		pCommandList->SetComputeRootDescriptorTable(2, m_blur1.UavGpuDescriptorHandle);
		pCommandList->Dispatch(groupXCount, static_cast<UINT>(m_blur0.Height), 1);

		// Change states of blur0 to Unordered Access and blur1 to Generic Read:
		{
			std::array<CD3DX12_RESOURCE_BARRIER, 2> barriers =
			{
				CD3DX12_RESOURCE_BARRIER::Transition(m_blur0.Resource.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
				CD3DX12_RESOURCE_BARRIER::Transition(m_blur1.Resource.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ)
			};
			pCommandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
		}

		pCommandList->SetPipelineState(verticalBlurPSO);
		pCommandList->SetComputeRootDescriptorTable(1, m_blur1.SrvGpuDescriptorHandle);
		pCommandList->SetComputeRootDescriptorTable(2, m_blur0.UavGpuDescriptorHandle);
		pCommandList->Dispatch(static_cast<UINT>(m_blur0.Width), groupYCount, 1);

		// Change states of blur0 to Generic Read and blur1 to Unordered Access:
		{
			std::array<CD3DX12_RESOURCE_BARRIER, 2> barriers =
			{
				CD3DX12_RESOURCE_BARRIER::Transition(m_blur0.Resource.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ),
				CD3DX12_RESOURCE_BARRIER::Transition(m_blur1.Resource.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
			};
			pCommandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
		}
	}
	
	// Copy result back to input:
	{
		std::array<CD3DX12_RESOURCE_BARRIER, 2> barriers =
		{
			CD3DX12_RESOURCE_BARRIER::Transition(m_blur0.Resource.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_SOURCE),
			CD3DX12_RESOURCE_BARRIER::Transition(pInput, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST)
		};
		pCommandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
		pCommandList->CopyResource(pInput, m_blur0.Resource.Get());
	}

	// Restore initial states:
	{
		std::array<CD3DX12_RESOURCE_BARRIER, 3> barriers =
		{
			CD3DX12_RESOURCE_BARRIER::Transition(m_blur0.Resource.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COMMON),
			CD3DX12_RESOURCE_BARRIER::Transition(m_blur1.Resource.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON),
			CD3DX12_RESOURCE_BARRIER::Transition(pInput, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET)
		};
		pCommandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
	}
}

float BlurFilter::CalculateGaussianWeight(float x, float sigma)
{
	return std::expf(- x*x / (2.0f * sigma * sigma));
}
std::array<float, 9> BlurFilter::CalculateGaussianWeights(float sigma)
{
	std::array<float, 9> weights = 
	{
		CalculateGaussianWeight(-4, sigma),
		CalculateGaussianWeight(-3, sigma),
		CalculateGaussianWeight(-2, sigma),
		CalculateGaussianWeight(-1, sigma),
		CalculateGaussianWeight(0, sigma),
		CalculateGaussianWeight(1, sigma),
		CalculateGaussianWeight(2, sigma),
		CalculateGaussianWeight(3, sigma),
		CalculateGaussianWeight(4, sigma)
	};

	// Calculate the sum of all weights:
	auto sum = 0.0f;
	for(auto weight : weights)
		sum += weight;

	// Normalize weights:
	for (auto& weight : weights)
		weight /= sum;

	return weights;
}

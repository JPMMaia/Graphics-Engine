#pragma once

#include "RWTexture.h"

#include <array>
#include <cstdint>
#include <d3d12.h>

namespace GraphicsEngine
{
	class BlurFilter
	{
	public:
		BlurFilter() = default;
		BlurFilter(const D3DBase& d3dBase, DescriptorHeap* pDescriptorHeap, uint32_t width, uint32_t height, DXGI_FORMAT format);

		void Execute(ID3D12GraphicsCommandList* pCommandList, ID3D12RootSignature* pRootSignature, ID3D12Resource* pInput, ID3D12PipelineState* horizontalBlurPSO, ID3D12PipelineState* verticalBlurPSO, size_t blurCount) const;

	private:
		static float CalculateGaussianWeight(float x, float sigma);
		static std::array<float, 9> CalculateGaussianWeights(float sigma);

	private:
		RWTexture m_blur0;
		RWTexture m_blur1;
	};
}

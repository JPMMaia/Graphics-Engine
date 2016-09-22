#pragma once

#include "RWTexture.h"

#include <d3d12.h>
#include <stdint.h>

namespace GraphicsEngine
{
	class BlurFilter
	{
	public:
		BlurFilter() = default;
		BlurFilter(const D3DBase& d3dBase, DescriptorHeap* pDescriptorHeap, uint32_t width, uint32_t height, DXGI_FORMAT format);

		void Execute(ID3D12GraphicsCommandList* pCommandList, ID3D12RootSignature* pRootSignature, ID3D12Resource* pInput, ID3D12PipelineState* horizontalBlurPSO) const;

	private:
		RWTexture m_blur0;
		RWTexture m_blur1;
	};
}

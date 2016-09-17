#pragma once

#include "RWTexture.h"

#include <d3d12.h>
#include <wrl/client.h>

namespace GraphicsEngine
{
	class BlurFilter
	{
	public:
		BlurFilter() = default;
		BlurFilter(ID3D12Device* d3dDevice, uint32_t width, uint32_t height, DXGI_FORMAT format, CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescriptor, UINT descriptorIncrementSize);

	private:
		RWTexture m_blur0;
		RWTexture m_blur1;
	};
}

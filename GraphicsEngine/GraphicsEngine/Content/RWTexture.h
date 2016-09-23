#pragma once

#include "d3dx12.h"

#include <d3d12.h>
#include <wrl/Client.h>

namespace GraphicsEngine
{
	class D3DBase;
	class DescriptorHeap;

	struct RWTexture
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> Resource;
		UINT64 Width;
		UINT64 Height;
		DXGI_FORMAT Format;

		CD3DX12_CPU_DESCRIPTOR_HANDLE SrvCpuDescriptorHandle;
		CD3DX12_GPU_DESCRIPTOR_HANDLE SrvGpuDescriptorHandle;
		CD3DX12_CPU_DESCRIPTOR_HANDLE UavCpuDescriptorHandle;
		CD3DX12_GPU_DESCRIPTOR_HANDLE UavGpuDescriptorHandle;

	public:
		RWTexture() = default;
		RWTexture(const D3DBase& d3dBase, DescriptorHeap* pDescriptorHeap, UINT64 width, UINT64 height, DXGI_FORMAT format);

	private:
		void BuildResource(const D3DBase& d3dBase, UINT64 width, UINT64 height, DXGI_FORMAT format);
		void BuildDescriptors(const D3DBase& d3dBase, DescriptorHeap* pDescriptorHeap);
	};
}

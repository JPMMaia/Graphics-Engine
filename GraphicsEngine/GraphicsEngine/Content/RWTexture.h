#pragma once

namespace GraphicsEngine
{
	struct RWTexture
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> Resource;
		UINT64 Width;
		UINT64 Height;
		DXGI_FORMAT Format;

	public:
		RWTexture() = default;
		RWTexture(ID3D12Device* device, UINT64 width, UINT64 height, DXGI_FORMAT format, CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescriptor, UINT descriptorIncrementSize);

	private:
		void BuildResource(ID3D12Device* device, UINT64 width, UINT64 height, DXGI_FORMAT format);
		void BuildDescriptors(ID3D12Device* device, CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescriptor, UINT descriptorIncrementSize) const;
	};
}

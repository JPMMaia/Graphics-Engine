#pragma once

#include <wrl/client.h>
#include <d3d12.h>

namespace GraphicsEngine
{
	class D3DBase;

	class DescriptorHeap
	{
	public:
		DescriptorHeap() = default;
		DescriptorHeap(const D3DBase& d3dBase, size_t capacity);

		INT CreateShaderResourceView(const D3DBase& d3dBase, ID3D12Resource* pResource, const D3D12_SHADER_RESOURCE_VIEW_DESC* pDescription);
		INT CreateUnorderedAccessView(const D3DBase& d3dBase, ID3D12Resource* pResource, ID3D12Resource *pCounterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* pDescription);

		ID3D12DescriptorHeap* Get() const;
		size_t GetDescriptorCount() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
		size_t m_capacity;
		size_t m_count;
	};
}

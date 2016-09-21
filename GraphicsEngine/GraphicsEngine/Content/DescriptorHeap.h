#pragma once

#include "Texture.h"

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

		INT CreateShaderResourceView(const D3DBase& d3dBase, const Texture& texture);

		ID3D12DescriptorHeap* Get() const;
		size_t GetDescriptorCount() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
		size_t m_capacity;
		size_t m_count;
	};
}

#include "stdafx.h"
#include "TextureHeap.h"

using namespace GraphicsEngine;
using namespace std;

void TextureHeap::AddTexture(unique_ptr<Texture>&& texture)
{
	m_textures[texture->Name] = std::move(texture);
}

void TextureHeap::Create(const D3DBase& d3dBase)
{
	D3D12_DESCRIPTOR_HEAP_DESC texturesHeapDescription = {};
	texturesHeapDescription.NumDescriptors = static_cast<UINT>(m_textures.size());
	texturesHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	texturesHeapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	texturesHeapDescription.NodeMask = 0;

	auto device = d3dBase.GetDevice();
	DX::ThrowIfFailed(
		device->CreateDescriptorHeap(&texturesHeapDescription, IID_PPV_ARGS(m_textureDescriptorHeap.GetAddressOf()))
		);

	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(m_textureDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// Create texture descriptors:
	for(auto&& pair : m_textures)
	{
		auto&& texture = pair.second->Resource;

		D3D12_SHADER_RESOURCE_VIEW_DESC textureDescription = {};
		textureDescription.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		textureDescription.Format = texture->GetDesc().Format;
		textureDescription.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
		textureDescription.Texture2D.MipLevels = texture->GetDesc().MipLevels;
		textureDescription.Texture2D.MostDetailedMip = 0;
		textureDescription.Texture2D.ResourceMinLODClamp = 0.0f;

		device->CreateShaderResourceView(texture.Get(), &textureDescription, descriptorHandle);

		// Increment descriptor handle:
		descriptorHandle.Offset(1, d3dBase.GetCbvSrvUavDescriptorSize());
	}
}

ID3D12DescriptorHeap* TextureHeap::GetDescriptorHeap() const
{
	return m_textureDescriptorHeap.Get();
}

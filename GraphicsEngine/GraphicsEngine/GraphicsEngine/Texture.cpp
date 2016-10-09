#include "stdafx.h"
#include "Texture.h"

#include "DDSTextureLoader.h"
#include "DescriptorHeap.h"

using namespace Common;
using namespace GraphicsEngine;

Texture::Texture(const std::string& name, const std::wstring& filename) :
	Name(name),
	Filename(filename)
{
}

void Texture::Load(const D3DBase& d3dBase)
{
	ThrowIfFailed(
		DirectX::CreateDDSTextureFromFile12(
			d3dBase.GetDevice(),
			d3dBase.GetCommandList(),
			this->Filename.c_str(),
			this->Resource,
			this->UploadHeap
			)
		);
}

void Texture::CreateShaderResourceView(const D3DBase& d3dBase, DescriptorHeap* pDescriptorHeap)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC textureViewDescription = {};
	textureViewDescription.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	textureViewDescription.Format = this->Resource->GetDesc().Format;
	textureViewDescription.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
	textureViewDescription.Texture2D.MipLevels = this->Resource->GetDesc().MipLevels;
	textureViewDescription.Texture2D.MostDetailedMip = 0;
	textureViewDescription.Texture2D.ResourceMinLODClamp = 0.0f;

	this->HeapIndex = pDescriptorHeap->CreateShaderResourceView(d3dBase, this->Resource.Get(), &textureViewDescription);
}

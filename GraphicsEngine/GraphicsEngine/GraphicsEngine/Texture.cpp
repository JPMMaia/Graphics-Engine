#include "stdafx.h"
#include "Texture.h"

#include <cwctype>
#include <DirectXTex/DDSTextureLoader/DDSTextureLoader.h>
#include <DirectXTex/DirectXTex/DirectXTex.h>
#include <wincodec.h>

using namespace Common;
using namespace DirectX;
using namespace GraphicsEngine;

Texture::Texture(ID3D11Device* device, const std::string& name, const std::wstring& filename) :
	m_name(name)
{
	CreateTextureFromFile(device, filename);
}
Texture::Texture(ID3D11Device* device, const std::string& name, ID3D11Resource* texture, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDescription) :
	m_name(name)
{
	ThrowIfFailed(device->CreateShaderResourceView(texture, &srvDescription, m_textureView.GetAddressOf()));
}

void Texture::Initialize(ID3D11Device* device, const std::string& name, const std::wstring& filename)
{
	m_name = name;
	CreateTextureFromFile(device, filename);
}
void Texture::Reset()
{
	m_textureView.Reset();
}

ID3D11ShaderResourceView* Texture::Get() const
{
	return m_textureView.Get();
}
ID3D11ShaderResourceView* const* Texture::GetAddressOf() const
{
	return m_textureView.GetAddressOf();
}

void Texture::CreateTextureFromFile(ID3D11Device* device, const std::wstring& filename)
{
	auto fileExtension = Common::Helpers::GetFileExtension(filename);
	std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), std::towlower);

	ScratchImage scratchImage;
	TexMetadata metadata;
	if (fileExtension == L"dds")
	{
		ThrowIfFailed(LoadFromDDSFile(filename.c_str(), DDS_FLAGS_NONE, &metadata, scratchImage));
	}
	else if (fileExtension == L"hdr")
	{
		ThrowIfFailed(LoadFromHDRFile(filename.c_str(), &metadata, scratchImage));
	}
	else if (fileExtension == L"tga")
	{
		ThrowIfFailed(LoadFromTGAFile(filename.c_str(), &metadata, scratchImage));
	}
	else
	{
		ThrowIfFailed(LoadFromWICFile(filename.c_str(), WIC_FLAGS_NONE, &metadata, scratchImage));
	}
	
	ThrowIfFailed(CreateShaderResourceView(device, scratchImage.GetImages(), scratchImage.GetImageCount(), metadata, m_textureView.GetAddressOf()));
}

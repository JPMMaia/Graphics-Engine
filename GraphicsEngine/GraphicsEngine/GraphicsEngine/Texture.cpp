#include "stdafx.h"
#include "Texture.h"

#include <DirectXTex/DDSTextureLoader/DDSTextureLoader.h>

using namespace Common;
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
	// Create shader resource view from a DDS Texture file:
	ThrowIfFailed(
		DirectX::CreateDDSTextureFromFile(device, filename.data(), nullptr, m_textureView.GetAddressOf())
	);
}
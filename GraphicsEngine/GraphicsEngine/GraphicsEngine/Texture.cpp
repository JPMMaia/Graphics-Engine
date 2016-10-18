#include "stdafx.h"
#include "Texture.h"

#include <DirectXTex/DDSTextureLoader/DDSTextureLoader.h>

using namespace Common;
using namespace GraphicsEngine;

Texture::Texture()
{
}
Texture::Texture(ID3D11Device* device, const std::wstring& filename)
{
	Initialize(device, filename);
}

void Texture::Initialize(ID3D11Device* device, const std::wstring& filename)
{
	// Create shader resource view fromt a DDS Texture file:
	ThrowIfFailed(
		DirectX::CreateDDSTextureFromFile(device, filename.data(), nullptr, m_textureView.GetAddressOf())
		);
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
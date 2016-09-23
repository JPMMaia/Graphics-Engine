#include "stdafx.h"
#include "TextureManager.h"
#include "DescriptorHeap.h"

using namespace GraphicsEngine;

void TextureManager::AddTexture(std::unique_ptr<Texture>&& texture)
{
	m_textures[texture->Name] = std::move(texture);
}

void TextureManager::LoadAllTextures(const D3DBase& d3dBase)
{
	for(auto&& pair : m_textures)
	{
		pair.second->Load(d3dBase);
	}
}

void TextureManager::CreateShaderResourceViews(const D3DBase& d3dBase, DescriptorHeap* pDescriptorHeap)
{
	for(auto&& pair : m_textures)
	{
		auto pTexture = pair.second.get();
		pTexture->CreateShaderResourceView(d3dBase, pDescriptorHeap);
	}
}

Texture* TextureManager::GetTexture(const std::string& name) const
{
	return m_textures.at(name).get();
}

size_t TextureManager::GetTextureCount() const
{
	return m_textures.size();
}

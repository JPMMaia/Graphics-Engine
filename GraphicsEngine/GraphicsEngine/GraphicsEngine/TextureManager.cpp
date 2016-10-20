#include "stdafx.h"
#include "TextureManager.h"

using namespace GraphicsEngine;

void TextureManager::Reset()
{
	for (auto pair : m_textures)
		pair.second.Reset();

	m_textures.clear();
}

bool TextureManager::Create(ID3D11Device* d3dDevice, const std::wstring& textureID, const std::wstring& textureUrl)
{
	// If there textureID already exists, return false:
	if (m_textures.find(textureID) != m_textures.end())
		return false;

	m_textures[textureID] = Texture(d3dDevice, textureUrl);

	return true;
}

const Texture& TextureManager::operator[](const std::wstring& textureID) const
{
	return m_textures.at(textureID);
}

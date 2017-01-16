#include "stdafx.h"
#include "TextureManager.h"

using namespace GraphicsEngine;

void TextureManager::Reset()
{
	for (auto& pair : m_textures)
		pair.second.Reset();

	m_textures.clear();
}

bool TextureManager::Add(const std::string& name, Texture&& texture)
{
	// If it already exists, return false:
	if (m_textures.find(name) != m_textures.end())
		return false;

	// Add texture:
	m_textures.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(texture));

	return true;
}
bool TextureManager::Create(ID3D11Device* d3dDevice, const std::string& name, const std::wstring& textureUrl)
{
	// If it already exists, return false:
	if (m_textures.find(name) != m_textures.end())
		return false;

	// Create a new texture:
	m_textures.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(d3dDevice, name, textureUrl));

	return true;
}

const Texture& TextureManager::operator[](const std::string& name) const
{
	return m_textures.at(name);
}

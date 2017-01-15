#include "stdafx.h"
#include "TextureArray.h"

using namespace GraphicsEngine;

void TextureArray::Add(ID3D11ShaderResourceView* textureSRV)
{
	m_shaderResourceViews.push_back(textureSRV);
}

ID3D11ShaderResourceView* const* TextureArray::GetTextureArray() const
{
	return m_shaderResourceViews.data();
}
ID3D11ShaderResourceView* TextureArray::GetTexture(size_t index) const
{
	return m_shaderResourceViews[index];
}
size_t TextureArray::GetSize() const
{
	return m_shaderResourceViews.size();
}

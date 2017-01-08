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

size_t TextureArray::GetSize() const
{
	return m_shaderResourceViews.size();
}

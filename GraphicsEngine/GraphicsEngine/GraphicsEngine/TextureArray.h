#pragma once

#include "Texture.h"
#include <vector>

namespace GraphicsEngine
{
	class TextureArray
	{
	public:
		void Add(ID3D11ShaderResourceView* textureSRV);

		ID3D11ShaderResourceView* const* GetTextureArray() const;
		size_t GetSize() const;

	private:
		std::vector<ID3D11ShaderResourceView*> m_shaderResourceViews;
	};
}

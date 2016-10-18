#pragma once

#include "Texture.h"

#include <map>

namespace GraphicsEngine
{
	class TextureManager
	{
	public:
		void Reset();

		bool Create(ID3D11Device* d3dDevice, const std::wstring& textureID, const std::wstring& textureUrl);

		const Texture& operator[](const std::wstring& textureID) const;

	private:
		std::map<std::wstring, Texture> m_textures;
	};
}
#pragma once

#include "Texture.h"

#include <unordered_map>

namespace GraphicsEngine
{
	class TextureManager
	{
	public:
		void Reset();

		bool Add(const std::string& name, Texture&& texture);
		bool Create(ID3D11Device* d3dDevice, const std::string& name, const std::wstring& textureUrl);

		const Texture& operator[](const std::string& name) const;

	private:
		std::unordered_map<std::string, Texture> m_textures;
	};
}
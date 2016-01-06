#pragma once

#include "Material.h"
#include "Texture.h"

namespace GraphicsEngine
{
	struct TextureAppearance
	{
		TextureAppearance(const Material& material, const Texture& textureMap, const Texture& normalMap) :
			Material(material),
			TextureMap(textureMap),
			NormalMap(normalMap)
		{
		}

		Material Material;
		const Texture& TextureMap;
		const Texture& NormalMap;
	};
}
#pragma once

#include "Material.h"
#include "Texture.h"

namespace GraphicsEngine
{
	struct TextureAppearance
	{
		TextureAppearance(const Material& material, const Texture& textureMap, const Texture& normalMap, const Texture& heightMap) :
			Material(material),
			TextureMap(textureMap),
			NormalMap(normalMap),
			HeightMap(heightMap)
		{
		}

		Material Material;
		const Texture& TextureMap;
		const Texture& NormalMap;
		const Texture& HeightMap;
	};
}
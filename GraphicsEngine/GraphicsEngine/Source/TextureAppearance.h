#pragma once

#include "Material.h"
#include "Texture.h"

namespace GraphicsEngine
{
	struct TextureAppearance
	{
		TextureAppearance(const Material& material, const Texture& texture) :
			Material(material),
			Texture(texture)
		{
		}

		Material Material;
		const Texture& Texture;
	};
}
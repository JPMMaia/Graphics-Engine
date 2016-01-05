#pragma once

#include <Source/TextureManager.h>

namespace GraphicsEngine
{
	class LightModel;

	class ModelBuilder
	{
	public:
		ModelBuilder(TextureManager& textureManager);

		LightModel CreateFromX3D(ID3D11Device* d3dDevice, const std::wstring& x3dFilename, uint32_t maxInstanceCount) const;

	private:
		TextureManager& m_textureManager;
	};
}

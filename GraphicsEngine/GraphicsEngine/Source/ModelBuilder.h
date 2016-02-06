#pragma once

#include "LightEffect.h"
#include "TextureManager.h"
#include "TerrainModel.h"

namespace GraphicsEngine
{
	class LightModel;

	class ModelBuilder
	{
	public:
		ModelBuilder(TextureManager& textureManager);

		LightModel CreateFromX3D(ID3D11Device* d3dDevice, const std::wstring& x3dFilename, const std::vector<LightEffect::InstanceData>& instancedData) const;
		LightModel CreateLightCube(ID3D11Device* d3dDevice, const std::vector<LightEffect::InstanceData>& instancedData) const;
		TerrainModel CreateTerrain(ID3D11Device* d3dDevice, float width, float depth, uint32_t xCellCount, uint32_t zCellCount) const;

	private:
		TextureManager& m_textureManager;
	};
}

#pragma once

#include "Model.h"
#include "VertexTypes.h"
#include "TextureAppearance.h"
#include "TerrainEffect.h"

namespace GraphicsEngine
{
	class TerrainModel
	{
	public:
		TerrainModel();
		TerrainModel(ID3D11Device* d3dDevice, const std::vector<VertexPositionTexture>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets, const std::vector<TextureAppearance>& materials, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology);

		void Initialize(ID3D11Device* d3dDevice, const std::vector<VertexPositionTexture>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets, const std::vector<TextureAppearance>& materials, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology);
		void Reset();

		void Draw(ID3D11DeviceContext1* d3dDeviceContext, TerrainEffect& terrainEffect, uint32_t visibleInstanceCount) const;

	private:
		Model<VertexPositionTexture, uint32_t> m_model;
		std::vector<TextureAppearance> m_materials;
	};
}
#include "stdafx.h"
#include "TerrainModel.h"

using namespace GraphicsEngine;

TerrainModel::TerrainModel()
{
}
TerrainModel::TerrainModel(ID3D11Device* d3dDevice, const std::vector<VertexPositionTexture>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets, const std::vector<TextureAppearance>& materials, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology) :
m_model(d3dDevice, vertices, indices, subsets, primitiveTopology),
m_materials(materials)
{
}

void TerrainModel::Initialize(ID3D11Device* d3dDevice, const std::vector<VertexPositionTexture>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets, const std::vector<TextureAppearance>& materials, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology)
{
	m_model.Initialize(d3dDevice, vertices, indices, subsets, primitiveTopology);
	m_materials.assign(materials.begin(), materials.end());
}
void TerrainModel::Reset()
{
	m_model.Reset();
}

void TerrainModel::Draw(ID3D11DeviceContext1* d3dDeviceContext, TerrainEffect& terrainEffect, uint32_t visibleInstanceCount) const
{
	auto& mesh = m_model.Mesh;
	auto& subsets = m_model.Subsets;

	for (size_t i = 0; i < subsets.size(); ++i)
	{
		// Set subset's material:
		auto material = m_materials[i];
		terrainEffect.UpdateSubsetConstantBuffer(d3dDeviceContext, { material.Material });

		// Draw subset:
		auto& subset = subsets[i];
		mesh.Draw(d3dDeviceContext, subset.IndexCount, subset.StartIndex);
	}
}

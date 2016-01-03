#include "stdafx.h"
#include "LightModel.h"

using namespace GraphicsEngine;

LightModel::LightModel()
{
}
LightModel::LightModel(ID3D11Device* d3dDevice, const std::vector<VertexPositionNormalTexture>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets, const std::vector<TextureAppearance>& materials) :
	m_model(d3dDevice, vertices, indices, subsets),
	m_materials(materials)
{
}

void LightModel::Initialize(ID3D11Device* d3dDevice, const std::vector<VertexPositionNormalTexture>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets, const std::vector<TextureAppearance>& materials)
{
	m_model.Initialize(d3dDevice, vertices, indices, subsets);
	m_materials.assign(materials.begin(), materials.end());
}
void LightModel::Reset()
{
	m_model.Reset();
}

void LightModel::Draw(ID3D11DeviceContext1* d3dDeviceContext, LightEffect& lightEffect) const
{
	auto& mesh = m_model.Mesh;
	auto& subsets = m_model.Subsets;

	for (size_t i = 0; i < subsets.size(); ++i)
	{
		// Set subset's material:
		auto material = m_materials[i];
		lightEffect.UpdatePerSubsetConstantBuffer(d3dDeviceContext, { material.Material });
		lightEffect.SetTexture(d3dDeviceContext, material.Texture);

		// Draw subset:
		auto& subset = subsets[i];
		mesh.Draw(d3dDeviceContext, subset.IndexCount, subset.StartIndex);
	}
}
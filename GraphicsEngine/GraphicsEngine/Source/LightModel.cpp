#include "stdafx.h"
#include "LightModel.h"

using namespace GraphicsEngine;

LightModel::LightModel()
{
}
LightModel::LightModel(ID3D11Device* d3dDevice, const std::vector<VertexPositionTextureNormalTangent>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets, const std::vector<TextureAppearance>& materials, const std::vector<InstancedDataTypes::World>& instancedData, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology) :
	m_model(d3dDevice, vertices, indices, subsets, primitiveTopology),
	m_materials(materials),
	m_instancedData(d3dDevice, instancedData)
{
}

void LightModel::Initialize(ID3D11Device* d3dDevice, const std::vector<VertexPositionTextureNormalTangent>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets, const std::vector<TextureAppearance>& materials, const std::vector<InstancedDataTypes::World>& instancedData, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology)
{
	m_model = Model<VertexPositionTextureNormalTangent, uint32_t>(d3dDevice, vertices, indices, subsets, primitiveTopology);
	m_materials.assign(materials.begin(), materials.end());
	m_instancedData = InstanceBuffer(d3dDevice, instancedData);
}
void LightModel::Reset()
{
	m_model.Reset();
}

void LightModel::Draw(ID3D11DeviceContext1* d3dDeviceContext, LightEffect& lightEffect, uint32_t visibleInstanceCount) const
{
	auto& mesh = m_model.Mesh;
	auto& subsets = m_model.Subsets;

	for (size_t i = 0; i < subsets.size(); ++i)
	{
		// Set subset's material:
		auto material = m_materials[i];
		lightEffect.UpdateSubsetBuffer(d3dDeviceContext, { material.Material });
		lightEffect.SetTextureMap(d3dDeviceContext, material.TextureMap);
		lightEffect.SetNormalMap(d3dDeviceContext, material.NormalMap);
		lightEffect.SetHeightMap(d3dDeviceContext, material.HeightMap);

		// Draw subset:
		auto& subset = subsets[i];
		mesh.Draw(d3dDeviceContext, m_instancedData, subset.IndexCount, visibleInstanceCount, subset.StartIndex);
	}
}

const InstanceBuffer& LightModel::GetInstancedData() const
{
	return m_instancedData;
}

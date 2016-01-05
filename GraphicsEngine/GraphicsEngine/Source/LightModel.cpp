#include "stdafx.h"
#include "LightModel.h"

using namespace GraphicsEngine;

LightModel::LightModel()
{
}
LightModel::LightModel(ID3D11Device* d3dDevice, const std::vector<VertexPositionNormalTexture>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets, const std::vector<TextureAppearance>& materials, uint32_t maxInstanceCount) :
	m_model(d3dDevice, vertices, indices, subsets),
	m_materials(materials),
	m_instancedData(d3dDevice, maxInstanceCount * sizeof(LightEffect::InstanceData), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE)
{
}

void LightModel::Initialize(ID3D11Device* d3dDevice, const std::vector<VertexPositionNormalTexture>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets, const std::vector<TextureAppearance>& materials, uint32_t maxInstanceCount)
{
	m_model.Initialize(d3dDevice, vertices, indices, subsets);
	m_materials.assign(materials.begin(), materials.end());
	m_instancedData.Initialize(d3dDevice, maxInstanceCount * sizeof(LightEffect::InstanceData), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
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
		lightEffect.UpdateSubsetConstantBuffer(d3dDeviceContext, { material.Material });
		lightEffect.SetTexture(d3dDeviceContext, material.Texture);

		// Draw subset:
		auto& subset = subsets[i];
		mesh.Draw(d3dDeviceContext, m_instancedData, subset.IndexCount, visibleInstanceCount, subset.StartIndex);
	}
}

const Buffer& LightModel::GetInstancedData() const
{
	return m_instancedData;
}

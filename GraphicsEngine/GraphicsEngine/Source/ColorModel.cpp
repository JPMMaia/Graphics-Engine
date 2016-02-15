#include "stdafx.h"
#include "ColorModel.h"

using namespace GraphicsEngine;

ColorModel::ColorModel()
{
}
ColorModel::ColorModel(ID3D11Device* d3dDevice, const std::vector<VertexPositionColor>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets, const std::vector<InstancedDataTypes::World>& instancedData, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology) :
	m_model(d3dDevice, vertices, indices, subsets, primitiveTopology),
	m_instancedData(d3dDevice, instancedData)
{
}

void ColorModel::Reset()
{
	m_instancedData.Reset();
	m_model.Reset();
}

void ColorModel::Draw(ID3D11DeviceContext1* d3dDeviceContext, ColorEffect& colorEffect, uint32_t visibleInstanceCount) const
{
	auto& mesh = m_model.Mesh;
	auto& subsets = m_model.Subsets;

	for (size_t i = 0; i < subsets.size(); ++i)
	{
		// Draw subset:
		auto& subset = subsets[i];
		mesh.Draw(d3dDeviceContext, m_instancedData, subset.IndexCount, visibleInstanceCount, subset.StartIndex);
	}
}

const InstanceBuffer& ColorModel::GetInstancedData() const
{
	return m_instancedData;
}

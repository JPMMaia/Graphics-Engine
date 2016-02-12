#pragma once

#include "Model.h"
#include "VertexTypes.h"
#include "ColorEffect.h"
#include "TextureAppearance.h"

namespace GraphicsEngine
{
	class ColorModel
	{
	public:
		ColorModel(ID3D11Device* d3dDevice, const std::vector<VertexPositionColor>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets, const std::vector<ColorEffect::InstanceData>& instancedData, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology);

		void Reset();

		void Draw(ID3D11DeviceContext1* d3dDeviceContext, ColorEffect& colorEffect, uint32_t visibleInstanceCount) const;

		const InstanceBuffer& GetInstancedData() const;

	private:
		Model<VertexPositionColor, uint32_t> m_model;
		InstanceBuffer m_instancedData;
	};
}
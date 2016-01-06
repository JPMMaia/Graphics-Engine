#pragma once

#include "Model.h"
#include "VertexTypes.h"
#include "LightEffect.h"
#include "TextureAppearance.h"

namespace GraphicsEngine
{
	class LightModel
	{
	public:
		LightModel();
		LightModel(ID3D11Device* d3dDevice, const std::vector<VertexPositionTextureNormalTangent>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets, const std::vector<TextureAppearance>& materials, const std::vector<LightEffect::InstanceData>& instancedData);

		void Initialize(ID3D11Device* d3dDevice, const std::vector<VertexPositionTextureNormalTangent>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets, const std::vector<TextureAppearance>& materials, const std::vector<LightEffect::InstanceData>& instancedData);
		void Reset();

		void Draw(ID3D11DeviceContext1* d3dDeviceContext, LightEffect& lightEffect, uint32_t visibleInstanceCount) const;

		const InstanceBuffer& GetInstancedData() const;

	private:
		Model<VertexPositionTextureNormalTangent, uint32_t> m_model;
		std::vector<TextureAppearance> m_materials;

		InstanceBuffer m_instancedData;
	};
}
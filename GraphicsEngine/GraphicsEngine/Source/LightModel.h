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
		LightModel(ID3D11Device* d3dDevice, const std::vector<VertexPositionNormalTexture>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets, const std::vector<TextureAppearance>& materials, uint32_t maxInstanceCount);

		void Initialize(ID3D11Device* d3dDevice, const std::vector<VertexPositionNormalTexture>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets, const std::vector<TextureAppearance>& materials, uint32_t maxInstanceCount);
		void Reset();

		void Draw(ID3D11DeviceContext1* d3dDeviceContext, LightEffect& lightEffect, uint32_t visibleInstanceCount) const;

		const Buffer& GetInstancedData() const;

	private:
		Model<VertexPositionNormalTexture, uint32_t> m_model;
		std::vector<TextureAppearance> m_materials;

		Buffer m_instancedData;
	};
}
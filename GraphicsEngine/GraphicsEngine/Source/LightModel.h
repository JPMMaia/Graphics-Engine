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
		LightModel(ID3D11Device* d3dDevice, const std::vector<VertexPositionNormalTexture>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets, const std::vector<TextureAppearance>& materials);

		void Initialize(ID3D11Device* d3dDevice, const std::vector<VertexPositionNormalTexture>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets, const std::vector<TextureAppearance>& materials);
		void Reset();

		void Draw(ID3D11DeviceContext1* d3dDeviceContext, LightEffect& lightEffect) const;

	private:
		Model<VertexPositionNormalTexture, uint32_t> m_model;
		std::vector<TextureAppearance> m_materials;
	};
}
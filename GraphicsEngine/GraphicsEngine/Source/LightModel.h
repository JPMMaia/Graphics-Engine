#pragma once

#include "Model.h"
#include "VertexTypes.h"
#include "LightEffect.h"

namespace GraphicsEngine
{
	class LightModel
	{
	public:
		LightModel();
		LightModel(ID3D11Device* d3dDevice, const std::vector<VertexPositionNormalTexture>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets);

		void Initialize(ID3D11Device* d3dDevice, const std::vector<VertexPositionNormalTexture>& vertices, const std::vector<uint32_t>& indices, const std::vector<Subset>& subsets);
		void Reset();

		void Draw(ID3D11DeviceContext1* d3dDeviceContext, const LightEffect& lightEffect) const;

	private:
		Model<VertexPositionNormalTexture, uint32_t> m_model;
	};
}

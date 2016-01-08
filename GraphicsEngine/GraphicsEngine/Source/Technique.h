#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "RasterizerState.h"
#include "SamplerState.h"
#include "TechniqueArrays.h"
#include "DomainShader.h"
#include "HullShader.h"

namespace GraphicsEngine
{
	class Texture;

	class Technique
	{
	public:
		Technique();

		void Set(ID3D11DeviceContext1* d3dDeviceContext) const;

		void SetVertexShader(const VertexShader* vertexShader);
		void SetHullShader(const HullShader* hullShader);
		void SetDomainShader(const DomainShader* domainShader);
		void SetPixelShader(const PixelShader* pixelShader);
		void SetRasterizerState(const RasterizerState* rasterizerState);

		void VSSetConstantBuffer(ID3D11Buffer* constantBuffer, uint32_t slot);
		void DSSetConstantBuffer(ID3D11Buffer* constantBuffer, uint32_t slot);
		void PSSetConstantBuffer(ID3D11Buffer* constantBuffer, uint32_t slot);

		void VSSetSamplerState(const SamplerState& samplerState, uint32_t slot);
		void DSSetSamplerState(const SamplerState& samplerState, uint32_t slot);
		void PSSetSamplerState(const SamplerState& samplerState, uint32_t slot);

	private:
		const VertexShader* m_vertexShader;
		const HullShader* m_hullShader;
		const DomainShader* m_domainShader;
		const PixelShader* m_pixelShader;
		const RasterizerState* m_rasterizerState;

		TechniqueArrays<ID3D11Buffer*> m_constantBufferArrays;
		TechniqueArrays<ID3D11SamplerState*> m_samplerStateArrays;
	};
}

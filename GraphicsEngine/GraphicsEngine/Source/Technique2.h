#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "HullShader.h"
#include "DomainShader.h"
#include "RasterizerState.h"

namespace GraphicsEngine
{
	template<typename ResourceType>
	class TechniqueArray
	{
	public:
		TechniqueArray(
			void(__stdcall ID3D11DeviceContext1::* const& setResources)(UINT, UINT, ResourceType* const*),
			std::vector<ResourceType*>&& resources
			) :
			m_resources(std::forward<std::vector<ResourceType*>>(resources)),
			m_setResources(setResources)
		{
		}

		void SetResources(ID3D11DeviceContext1* d3dDeviceContext) const
		{
			(d3dDeviceContext->*m_setResources)(0, m_resources.size(), m_resources.data());
		}

	private:
		std::vector<ResourceType*> m_resources;
		void(__stdcall ID3D11DeviceContext1::* const& m_setResources)(UINT, UINT, ResourceType* const*);
	};

	using SamplerStateArray = TechniqueArray<ID3D11SamplerState>;
	using ConstantBufferArray = TechniqueArray<ID3D11Buffer>;

	class Technique2
	{
	public:
		Technique2() : 
			m_vertexShader(nullptr),
			m_hullShader(nullptr),
			m_domainShader(nullptr),
			m_pixelShader(nullptr),
			m_rasterizerState(nullptr)
		{
		};
		Technique2(const RasterizerState* rasterizerState, std::vector<ConstantBufferArray>&& constantBufferArrays, std::vector<SamplerStateArray>&& samplerStateArrays) :
			m_vertexShader(nullptr),
			m_hullShader(nullptr),
			m_domainShader(nullptr),
			m_pixelShader(nullptr),
			m_rasterizerState(rasterizerState),
			m_constantBufferArrays(std::forward<std::vector<ConstantBufferArray>>(constantBufferArrays)),
			m_samplerStateArrays(std::forward<std::vector<SamplerStateArray>>(samplerStateArrays))
		{
		}
		Technique2(const VertexShader* vertexShader, const HullShader* hullShader, const DomainShader* domainShader, const PixelShader* pixelShader, const RasterizerState* rasterizerState, std::vector<ConstantBufferArray>&& constantBufferArrays, std::vector<SamplerStateArray>&& samplerStateArrays) :
			m_vertexShader(vertexShader),
			m_hullShader(hullShader),
			m_domainShader(domainShader),
			m_pixelShader(pixelShader),
			m_rasterizerState(rasterizerState),
			m_constantBufferArrays(std::forward<std::vector<ConstantBufferArray>>(constantBufferArrays)),
			m_samplerStateArrays(std::forward<std::vector<SamplerStateArray>>(samplerStateArrays))
		{
		}

		void SetShader(const VertexShader* vertexShader)
		{
			m_vertexShader = vertexShader;
		}
		void SetShader(const HullShader* hullShader)
		{
			m_hullShader = hullShader;
		}
		void SetShader(const DomainShader* domainShader)
		{
			m_domainShader = domainShader;
		}
		void SetShader(const PixelShader* pixelShader)
		{
			m_pixelShader = pixelShader;
		}

		void Set(ID3D11DeviceContext1* d3dDeviceContext) const
		{
			// Set vertex shader:
			if (m_vertexShader)
				m_vertexShader->Set(d3dDeviceContext);
			else
				d3dDeviceContext->VSSetShader(nullptr, nullptr, 0);

			// Set hull shader:
			if (m_hullShader)
				m_hullShader->Set(d3dDeviceContext);
			else
				d3dDeviceContext->HSSetShader(nullptr, nullptr, 0);

			// Set domain shader:
			if (m_domainShader)
				m_domainShader->Set(d3dDeviceContext);
			else
				d3dDeviceContext->DSSetShader(nullptr, nullptr, 0);

			// Set pixel shader:
			if (m_pixelShader)
				m_pixelShader->Set(d3dDeviceContext);
			else
				d3dDeviceContext->PSSetShader(nullptr, nullptr, 0);

			// Set rasterizer state:
			if (m_rasterizerState)
				m_rasterizerState->Set(d3dDeviceContext);
			else
				d3dDeviceContext->RSSetState(nullptr);

			// Set constant buffers:
			for (auto& constantBufferArray : m_constantBufferArrays)
				constantBufferArray.SetResources(d3dDeviceContext);

			// Set sampler states:
			for (auto& samplerStateArray : m_samplerStateArrays)
				samplerStateArray.SetResources(d3dDeviceContext);
		}

	protected:
		const VertexShader* m_vertexShader;
		const HullShader* m_hullShader;
		const DomainShader* m_domainShader;
		const PixelShader* m_pixelShader;
		const RasterizerState* m_rasterizerState;
		std::vector<ConstantBufferArray> m_constantBufferArrays;
		std::vector<SamplerStateArray> m_samplerStateArrays;
	};
}

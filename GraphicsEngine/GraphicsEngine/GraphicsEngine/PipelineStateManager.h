#pragma once

#include "PipelineState.h"

#include <unordered_map>

namespace GraphicsEngine
{
	class D3DBase;

	class PipelineStateManager
	{
	public:
		PipelineStateManager() = default;
		explicit PipelineStateManager(const D3DBase& d3dBase);
		
		void SetPipelineState(ID3D11DeviceContext* deviceContext, const std::string& name) const;

		const PipelineState& GetPipelineState(const std::string& name) const;

	private:
		void InitializeShadersAndInputLayout(const D3DBase& d3dBase);
		void InitializeRasterizerStates(const D3DBase& d3dBase);
		void InitializeBlendStates(const D3DBase& d3dBase);
		void InitializeDepthStencilStates(const D3DBase& d3dBase);
		void InitializePipelineStateObjects();

	private:
		std::unordered_map<std::string, std::vector<D3D11_INPUT_ELEMENT_DESC>> m_inputLayouts;
		std::unordered_map<std::string, VertexShader> m_vertexShaders;
		std::unordered_map<std::string, HullShader> m_hullShaders;
		std::unordered_map<std::string, DomainShader> m_domainShaders;
		std::unordered_map<std::string, PixelShader> m_pixelShaders;
		std::unordered_map<std::string, RasterizerState> m_rasterizerStates;
		std::unordered_map<std::string, BlendState> m_blendStates;
		std::unordered_map<std::string, DepthStencilState> m_depthStencilStates;
		std::unordered_map<std::string, PipelineState> m_pipelineStateObjects;
	};
}

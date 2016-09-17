#pragma once

#include "Shader.h"

#include <unordered_map>

namespace GraphicsEngine
{
	class D3DBase;

	class PipelineStateManager
	{
	public:
		PipelineStateManager() = default;
		PipelineStateManager(const D3DBase& d3dBase, ID3D12RootSignature* rootSignature, ID3D12RootSignature* postProcessRootSignature);
		
		void SetPipelineState(ID3D12GraphicsCommandList* commandList, const std::string& name) const;

		ID3D12PipelineState* GetPipelineState(const std::string& name) const;

	private:
		void InitializeShadersAndInputLayout();
		void InitializePipelineStateObjects(const D3DBase& d3dBase, ID3D12RootSignature* rootSignature, ID3D12RootSignature* postProcessRootSignature);

	private:
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;
		std::unordered_map<std::string, Shader> m_shaders;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_pipelineStateObjects;
	};
}

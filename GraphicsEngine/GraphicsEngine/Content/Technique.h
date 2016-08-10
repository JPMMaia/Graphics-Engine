#pragma once
#include "Shader.h"

namespace GraphicsEngine
{
	class Technique
	{
	public:
		explicit Technique(const std::wstring& vertexShaderFilename, const std::wstring& pixelShaderFilename);
		Technique(const Technique& other) = delete;
		Technique& operator=(const Technique& other) = delete;

	protected:
		void InitializeRootSignature();
		void InitializePipelineState();
		void InitializeInputLayout();

	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		std::vector<D3D12_INPUT_LAYOUT_DESC> m_inputLayout;
		Shader m_vertexShader;
		Shader m_pixelShader;
	};
}

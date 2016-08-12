#pragma once

#include "MeshGeometry.h"
#include "Shader.h"
#include "UploadBuffer.h"
#include "DirectXMath.h"

#include <memory>

namespace GraphicsEngine
{
	class D3DBase;

	struct PerObjectCBType
	{
		DirectX::XMFLOAT4X4 WorldViewProjectionMatrix;
	};

	class Technique
	{
	public:
		Technique() = default;
		explicit Technique(const D3DBase& d3dBase);

		void Render(ID3D12GraphicsCommandList* commandList) const;

		void UpdatePerObjectCB(const PerObjectCBType& perObjectCB) const;

		ID3D12PipelineState* GetPipelineState() const;

	protected:
		void InitializeInputLayout();
		void InitializeRootSignature(const D3DBase& d3dBase);
		void InitializeDescriptorHeaps(const D3DBase& d3dBase);
		void InitializeConstantBuffers(const D3DBase& d3dBase);
		void InitializePipelineState(const D3DBase& d3dBase);

	private:
		Shader m_vertexShader;
		Shader m_pixelShader;

		std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
		std::unique_ptr<UploadBuffer<PerObjectCBType>> m_perObjectCB;
	};
}

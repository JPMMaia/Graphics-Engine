#include "stdafx.h"
#include "Technique.h"
#include "D3DBase.h"

using namespace GraphicsEngine;
using namespace Microsoft::WRL;
using namespace std;

Technique::Technique(const D3DBase& d3dBase) :
	m_vertexShader(L"ColorVertexShader.cso"),
	m_pixelShader(L"ColorPixelShader.cso")
{
	InitializeInputLayout();
	InitializeRootSignature(d3dBase);
	InitializeDescriptorHeaps(d3dBase);
	InitializeConstantBuffers(d3dBase);
	InitializePipelineState(d3dBase);
}

void Technique::Render(ID3D12GraphicsCommandList* commandList) const
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	commandList->SetGraphicsRootSignature(m_rootSignature.Get());
	commandList->SetGraphicsRootDescriptorTable(0, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
}

void Technique::UpdatePerObjectCB(const PerObjectCBType& perObjectCB) const
{
	m_perObjectCB->CopyData(0, perObjectCB);
}

ID3D12PipelineState* Technique::GetPipelineState() const
{
	return m_pipelineState.Get();
}

void Technique::InitializeInputLayout()
{
	m_inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}
void Technique::InitializeRootSignature(const D3DBase& d3dBase)
{
	// Root parameter can be a table, a root descriptor or a root constant:
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];

	// Create a single descriptor table of CBVs:
	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

	// A root signature is an array of root parameters:
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDescription(1, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// Create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer:
	{
		ComPtr<ID3DBlob> serializedRootSignature;
		ComPtr<ID3DBlob> errorBlob = nullptr;
		auto hr = D3D12SerializeRootSignature(&rootSignatureDescription, D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSignature.GetAddressOf(), errorBlob.GetAddressOf());
		if (errorBlob != nullptr)
			::OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
		DX::ThrowIfFailed(hr);

		DX::ThrowIfFailed(
			d3dBase.GetDevice()->CreateRootSignature(
				0,
				serializedRootSignature->GetBufferPointer(),
				serializedRootSignature->GetBufferSize(),
				IID_PPV_ARGS(m_rootSignature.GetAddressOf())
				)
			);
	}
}
void Technique::InitializeDescriptorHeaps(const D3DBase& d3dBase)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDescription;
	cbvHeapDescription.NumDescriptors = 1;
	cbvHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDescription.NodeMask = 0;

	DX::ThrowIfFailed(
		d3dBase.GetDevice()->CreateDescriptorHeap(&cbvHeapDescription, IID_PPV_ARGS(m_cbvHeap.GetAddressOf()))
		);
}
void Technique::InitializeConstantBuffers(const D3DBase& d3dBase)
{
	auto d3dDevice = d3dBase.GetDevice();

	m_perObjectCB = std::make_unique<UploadBuffer<PerObjectCBType>>(d3dDevice, 1, true);

	auto cbByteSize = DX::CalculateConstantBufferByteSize(sizeof(PerObjectCBType));
	auto boxCbIndex = 0;
	auto cbGpuAddress = m_perObjectCB->GetResource()->GetGPUVirtualAddress();
	cbGpuAddress += boxCbIndex * cbByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDescription;
	cbvDescription.BufferLocation = cbGpuAddress;
	cbvDescription.SizeInBytes = cbByteSize;

	d3dDevice->CreateConstantBufferView(&cbvDescription, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());
}
void Technique::InitializePipelineState(const D3DBase& d3dBase)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDescription = {};
	pipelineStateDescription.InputLayout.pInputElementDescs = &m_inputLayout[0];
	pipelineStateDescription.InputLayout.NumElements = static_cast<uint32_t>(m_inputLayout.size());
	pipelineStateDescription.pRootSignature = m_rootSignature.Get();
	pipelineStateDescription.VS = m_vertexShader.GetShaderBytecode();
	pipelineStateDescription.PS = m_pixelShader.GetShaderBytecode();
	pipelineStateDescription.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipelineStateDescription.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineStateDescription.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipelineStateDescription.SampleMask = UINT_MAX;
	pipelineStateDescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDescription.NumRenderTargets = 1;
	pipelineStateDescription.SampleDesc.Count = d3dBase.GetSampleCount();
	pipelineStateDescription.SampleDesc.Quality = d3dBase.GetSampleQuality();
	pipelineStateDescription.RTVFormats[0] = d3dBase.GetBackBufferFormat();
	pipelineStateDescription.DSVFormat = d3dBase.GetDepthStencilFormat();

	DX::ThrowIfFailed(
		d3dBase.GetDevice()->CreateGraphicsPipelineState(&pipelineStateDescription, IID_PPV_ARGS(m_pipelineState.GetAddressOf()))
		);
}


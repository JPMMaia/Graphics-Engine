#include "stdafx.h"
#include "PipelineStateManager.h"
#include "D3DBase.h"

using namespace GraphicsEngine;

PipelineStateManager::PipelineStateManager(const D3DBase& d3dBase, ID3D12RootSignature* rootSignature)
{
	InitializeShadersAndInputLayout();
	InitializePipelineStateObjects(d3dBase, rootSignature);
}

ID3D12PipelineState* PipelineStateManager::GetPipelineStateObject(const std::string& name) const
{
	return m_pipelineStateObjects.at(name).Get();
}

void PipelineStateManager::InitializeShadersAndInputLayout()
{
	m_shaders["StandardVS"] = Shader::CompileShader(L"Shaders/DefaultVertexShader.hlsl", nullptr, "main", "vs_5_1");

	const D3D_SHADER_MACRO opaqueDefines[] =
	{
		"FOG", "1",
		nullptr, nullptr
	};
	m_shaders["OpaquePS"] = Shader::CompileShader(L"Shaders/DefaultPixelShader.hlsl", opaqueDefines, "main", "ps_5_1");

	const D3D_SHADER_MACRO alphaTestedDefines[] =
	{
		"FOG", "1",
		"ALPHA_TEST", "1",
		nullptr, nullptr
	};
	m_shaders["AlphaTestedPS"] = Shader::CompileShader(L"Shaders/DefaultPixelShader.hlsl", alphaTestedDefines, "main", "ps_5_1");

	m_shaders["HorizontalBlurCS"] = Shader::CompileShader(L"Shaders/BlurComputeShader.hlsl", nullptr, "main", "cs_5_1");

	m_inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

void PipelineStateManager::InitializePipelineStateObjects(const D3DBase& d3dBase, ID3D12RootSignature* rootSignature)
{
	auto device = d3dBase.GetDevice();

	// Opaque:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePSODescription = {};
	{
		opaquePSODescription.InputLayout.pInputElementDescs = &m_inputLayout[0];
		opaquePSODescription.InputLayout.NumElements = static_cast<uint32_t>(m_inputLayout.size());
		opaquePSODescription.pRootSignature = rootSignature;
		opaquePSODescription.VS = m_shaders["StandardVS"].GetShaderBytecode();
		opaquePSODescription.PS = m_shaders["OpaquePS"].GetShaderBytecode();
		opaquePSODescription.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		opaquePSODescription.RasterizerState.FillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
		opaquePSODescription.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		opaquePSODescription.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		opaquePSODescription.SampleMask = UINT_MAX;
		opaquePSODescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		opaquePSODescription.NumRenderTargets = 1;
		opaquePSODescription.SampleDesc.Count = d3dBase.GetSampleCount();
		opaquePSODescription.SampleDesc.Quality = d3dBase.GetSampleQuality();
		opaquePSODescription.RTVFormats[0] = d3dBase.GetBackBufferFormat();
		opaquePSODescription.DSVFormat = d3dBase.GetDepthStencilFormat();

		DX::ThrowIfFailed(
			device->CreateGraphicsPipelineState(&opaquePSODescription, IID_PPV_ARGS(m_pipelineStateObjects["Opaque"].GetAddressOf()))
			);

		// Opaque wireframe:
		{
			auto opaqueWireframePSODescription = opaquePSODescription;
			opaqueWireframePSODescription.RasterizerState.FillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_WIREFRAME;
			DX::ThrowIfFailed(
				device->CreateGraphicsPipelineState(&opaqueWireframePSODescription, IID_PPV_ARGS(m_pipelineStateObjects["Opaque Wireframe"].GetAddressOf()))
				);
		}
	}

	// Transparent:
	auto transparentPSODescription = opaquePSODescription;
	{
		D3D12_RENDER_TARGET_BLEND_DESC blendDescription = {};
		blendDescription.BlendEnable = true;
		blendDescription.LogicOpEnable = false;
		blendDescription.SrcBlend = D3D12_BLEND::D3D12_BLEND_SRC_ALPHA;
		blendDescription.DestBlend = D3D12_BLEND::D3D12_BLEND_INV_SRC_ALPHA;
		blendDescription.BlendOp = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
		blendDescription.SrcBlendAlpha = D3D12_BLEND::D3D12_BLEND_ONE;
		blendDescription.DestBlendAlpha = D3D12_BLEND::D3D12_BLEND_ZERO;
		blendDescription.BlendOpAlpha = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
		blendDescription.LogicOp = D3D12_LOGIC_OP::D3D12_LOGIC_OP_NOOP;
		blendDescription.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_ALL;
		transparentPSODescription.BlendState.RenderTarget[0] = blendDescription;

		DX::ThrowIfFailed(
			device->CreateGraphicsPipelineState(&transparentPSODescription, IID_PPV_ARGS(m_pipelineStateObjects["Transparent"].GetAddressOf()))
			);

		// Transparent wireframe:
		{
			auto transparentWireframePSODescription = transparentPSODescription;
			transparentWireframePSODescription.RasterizerState.FillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_WIREFRAME;

			DX::ThrowIfFailed(
				device->CreateGraphicsPipelineState(&transparentWireframePSODescription, IID_PPV_ARGS(m_pipelineStateObjects["Transparent Wireframe"].GetAddressOf()))
				);
		}
	}

	// Alpha Tested:
	{
		auto alphaTestedPSODescription = opaquePSODescription;
		alphaTestedPSODescription.PS = m_shaders["AlphaTestedPS"].GetShaderBytecode();
		alphaTestedPSODescription.RasterizerState.CullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;

		DX::ThrowIfFailed(
			device->CreateGraphicsPipelineState(&alphaTestedPSODescription, IID_PPV_ARGS(m_pipelineStateObjects["Alpha Tested"].GetAddressOf()))
			);

		// Alpha tested wireframe
		{
			auto alphaTestedWireframePSODescription = alphaTestedPSODescription;
			alphaTestedWireframePSODescription.RasterizerState.FillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_WIREFRAME;

			DX::ThrowIfFailed(
				device->CreateGraphicsPipelineState(&alphaTestedWireframePSODescription, IID_PPV_ARGS(m_pipelineStateObjects["Alpha Tested Wireframe"].GetAddressOf()))
				);
		}
	}

	// Shadows:
	{
		D3D12_DEPTH_STENCIL_DESC shadowDepthStencilState;
		shadowDepthStencilState.DepthEnable = true;
		shadowDepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		shadowDepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		shadowDepthStencilState.StencilEnable = true;
		shadowDepthStencilState.StencilReadMask = 0xFF;
		shadowDepthStencilState.StencilWriteMask = 0xFF;

		shadowDepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		shadowDepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		shadowDepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
		shadowDepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

		shadowDepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		shadowDepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		shadowDepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
		shadowDepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

		auto shadowPSODescription = transparentPSODescription;
		shadowPSODescription.DepthStencilState = shadowDepthStencilState;
		DX::ThrowIfFailed(
			device->CreateGraphicsPipelineState(&shadowPSODescription, IID_PPV_ARGS(m_pipelineStateObjects["Shadow"].GetAddressOf()))
			);
	}

	D3D12_COMPUTE_PIPELINE_STATE_DESC horizontalBlurPipelineStateDescription = {};
	horizontalBlurPipelineStateDescription.pRootSignature = rootSignature;
	horizontalBlurPipelineStateDescription.CS = m_shaders["HorizontalBlurCS"].GetShaderBytecode();
	horizontalBlurPipelineStateDescription.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	DX::ThrowIfFailed(
		device->CreateComputePipelineState(&horizontalBlurPipelineStateDescription, IID_PPV_ARGS(m_pipelineStateObjects["HorizontalBlur"].GetAddressOf()))
		);
}

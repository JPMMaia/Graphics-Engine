﻿#include "stdafx.h"
#include "PipelineStateManager.h"
#include "D3DBase.h"
#include "RasterizerStateDescConstants.h"

#include <array>
#include "ShaderBufferTypes.h"
#include "BlendStateDescConstants.h"

using namespace Common;
using namespace GraphicsEngine;
using namespace std;

PipelineStateManager::PipelineStateManager(const D3DBase& d3dBase)
{
	InitializeShadersAndInputLayout(d3dBase);
	InitializeRasterizerStates(d3dBase);
	InitializeBlendStates(d3dBase);
	InitializePipelineStateObjects();
}

void PipelineStateManager::SetPipelineState(ID3D11DeviceContext* deviceContext, const std::string& name) const
{
	m_pipelineStateObjects.at(name).Set(deviceContext);
}
const PipelineState& PipelineStateManager::GetPipelineState(const std::string& name) const
{
	return m_pipelineStateObjects.at(name);
}

void PipelineStateManager::InitializeShadersAndInputLayout(const D3DBase& d3dBase)
{
	auto device = d3dBase.GetDevice();

	m_inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	auto maxNumLights = std::to_string(ShaderBufferTypes::PassData::MaxNumLights);
	std::array<D3D_SHADER_MACRO, 1> standardDefines =
	{
		"MAX_NUM_LIGHTS", maxNumLights.c_str()
	};
	m_vertexShaders["Standard"] = VertexShader(device, L"../GraphicsEngine/GraphicsEngine/Shaders/StandardVertexShader.hlsl", nullptr, "main", "vs_5_0", m_inputLayout);
	m_pixelShaders["Standard"] = PixelShader(device, L"../GraphicsEngine/GraphicsEngine/Shaders/StandardPixelShader.hlsl", nullptr, "main", "ps_5_0");
}
void PipelineStateManager::InitializeRasterizerStates(const D3DBase& d3dBase)
{
	auto device = d3dBase.GetDevice();

	m_rasterizerStates.emplace(std::piecewise_construct, std::forward_as_tuple("Default"), std::forward_as_tuple(device, RasterizerStateDescConstants::Default));
}
void PipelineStateManager::InitializeBlendStates(const D3DBase& d3dBase)
{
	auto device = d3dBase.GetDevice();

	auto defaultBlendFactor = std::array<FLOAT, 4>{ 1.0f, 1.0f, 1.0f, 1.0f };
	auto defaultSampleMask = 0xffffffff;

	m_blendStates.emplace(std::piecewise_construct, std::forward_as_tuple("Default"), std::forward_as_tuple(device, BlendStateDescConstants::Default(), defaultBlendFactor, defaultSampleMask));
	m_blendStates.emplace(std::piecewise_construct, std::forward_as_tuple("Transparent"), std::forward_as_tuple(device, BlendStateDescConstants::Transparent(), defaultBlendFactor, defaultSampleMask));
}
void PipelineStateManager::InitializePipelineStateObjects()
{
	// Opaque:
	PipelineState opaqueState;
	{
		opaqueState.VertexShader = &m_vertexShaders.at("Standard");
		opaqueState.PixelShader = &m_pixelShaders.at("Standard");
		opaqueState.RasterizerState = &m_rasterizerStates.at("Default");
		opaqueState.BlendState = &m_blendStates.at("Default");

		m_pipelineStateObjects.emplace("Opaque", opaqueState);
	}

	// Transparent:
	{
		auto transparentState = opaqueState;
		transparentState.BlendState = &m_blendStates.at("Transparent");

		m_pipelineStateObjects.emplace("Transparent", transparentState);
	}
}

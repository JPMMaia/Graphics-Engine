#include "stdafx.h"
#include "PipelineStateManager.h"
#include "D3DBase.h"
#include "RasterizerStateDescConstants.h"

#include <array>
#include "ShaderBufferTypes.h"
#include "BlendStateDescConstants.h"
#include "DepthStencilStateDescConstants.h"

using namespace Common;
using namespace GraphicsEngine;
using namespace std;

PipelineStateManager::PipelineStateManager(const D3DBase& d3dBase)
{
	InitializeShadersAndInputLayout(d3dBase);
	InitializeRasterizerStates(d3dBase);
	InitializeBlendStates(d3dBase);
	InitializeDepthStencilStates(d3dBase);
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

	m_inputLayouts["Default"] =
	{
		// Vertex data:
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		// Instance data:
		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	m_inputLayouts["SkyDome"] = {
		// Vertex data:
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		// Instance data:
		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	auto maxNumLights = std::to_string(ShaderBufferTypes::PassData::MaxNumLights);
	auto shadersFolderPath = wstring(L"../GraphicsEngine/GraphicsEngine/Shaders/");

	// Standard shaders:
	{
		std::array<D3D_SHADER_MACRO, 2> defines =
		{
			"MAX_NUM_LIGHTS", maxNumLights.c_str(),
			nullptr, nullptr
		};
		m_vertexShaders["Standard"] = VertexShader(device, shadersFolderPath + L"StandardVertexShader.hlsl", defines.data(), "main", "vs_5_0", m_inputLayouts["Default"]);
		m_pixelShaders["Standard"] = PixelShader(device, shadersFolderPath + L"StandardPixelShader.hlsl", defines.data(), "main", "ps_5_0");
	}
	
	// Standard alpha-clipped shaders:
	{
		std::array<D3D_SHADER_MACRO, 3> defines =
		{
			"MAX_NUM_LIGHTS", maxNumLights.c_str(),
			"ENABLE_ALPHA_CLIPPING", "1",
			nullptr, nullptr
		};
		m_vertexShaders["StandardAlphaClipped"] = VertexShader(device, shadersFolderPath + L"StandardVertexShader.hlsl", defines.data(), "main", "vs_5_0", m_inputLayouts["Default"]);
		m_pixelShaders["StandardAlphaClipped"] = PixelShader(device, shadersFolderPath + L"StandardPixelShader.hlsl", defines.data(), "main", "ps_5_0");
	}

	// Terrain shaders:
	m_vertexShaders["Terrain"] = VertexShader(device, shadersFolderPath + L"TerrainVertexShader.hlsl", nullptr, "main", "vs_5_0", m_inputLayouts["Default"]);
	m_hullShaders["Terrain"] = HullShader(device, shadersFolderPath + L"TerrainHullShader.hlsl", nullptr, "main", "hs_5_0");
	m_domainShaders["Terrain"] = DomainShader(device, shadersFolderPath + L"TerrainDomainShader.hlsl", nullptr, "main", "ds_5_0");
	m_pixelShaders["Terrain"] = PixelShader(device, shadersFolderPath + L"TerrainPixelShader.hlsl", nullptr, "main", "ps_5_0");

	// SkyDome shaders
	m_vertexShaders["SkyDome"] = VertexShader(device, shadersFolderPath + L"SkyDomeVertexShader.hlsl", nullptr, "main", "vs_5_0", m_inputLayouts["Default"]);
	m_pixelShaders["SkyDome"] = PixelShader(device, shadersFolderPath + L"SkyDomePixelShader.hlsl", nullptr, "main", "ps_5_0");
}
void PipelineStateManager::InitializeRasterizerStates(const D3DBase& d3dBase)
{
	auto device = d3dBase.GetDevice();
	
	m_rasterizerStates.emplace(std::piecewise_construct, std::forward_as_tuple("Default"), std::forward_as_tuple(device, RasterizerStateDescConstants::Default));
	m_rasterizerStates.emplace(std::piecewise_construct, std::forward_as_tuple("Wireframe"), std::forward_as_tuple(device, RasterizerStateDescConstants::Wireframe));
	m_rasterizerStates.emplace(std::piecewise_construct, std::forward_as_tuple("NoCulling"), std::forward_as_tuple(device, RasterizerStateDescConstants::NoCulling));
	
}
void PipelineStateManager::InitializeBlendStates(const D3DBase& d3dBase)
{
	auto device = d3dBase.GetDevice();

	auto defaultBlendFactor = std::array<FLOAT, 4>{ 1.0f, 1.0f, 1.0f, 1.0f };
	auto defaultSampleMask = 0xffffffff;

	m_blendStates.emplace(std::piecewise_construct, std::forward_as_tuple("Default"), std::forward_as_tuple(device, BlendStateDescConstants::Default(), defaultBlendFactor, defaultSampleMask));
	m_blendStates.emplace(std::piecewise_construct, std::forward_as_tuple("Transparent"), std::forward_as_tuple(device, BlendStateDescConstants::Transparent(), defaultBlendFactor, defaultSampleMask));
}
void PipelineStateManager::InitializeDepthStencilStates(const D3DBase& d3dBase)
{
	auto device = d3dBase.GetDevice();

	m_depthStencilStates.emplace(std::piecewise_construct, std::forward_as_tuple("Default"), std::forward_as_tuple(device, DepthStencilStateDescConstants::Default()));
	m_depthStencilStates.emplace(std::piecewise_construct, std::forward_as_tuple("DepthDisabled"), std::forward_as_tuple(device, DepthStencilStateDescConstants::DepthDisabled()));
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
		opaqueState.DepthStencilState = &m_depthStencilStates.at("Default");

		m_pipelineStateObjects.emplace("Opaque", opaqueState);
	}

	// Transparent:
	{
		auto transparentState = opaqueState;
		transparentState.BlendState = &m_blendStates.at("Transparent");

		m_pipelineStateObjects.emplace("Transparent", transparentState);
	}

	// Transparent:
	{
		auto transparentState = opaqueState;
		transparentState.BlendState = &m_blendStates.at("Transparent");

		m_pipelineStateObjects.emplace("Transparent", transparentState);
	}

	// Alpha-clipped:
	{
		auto alphaClippedState = opaqueState;
		alphaClippedState.VertexShader = &m_vertexShaders.at("StandardAlphaClipped");
		alphaClippedState.PixelShader = &m_pixelShaders.at("StandardAlphaClipped");
		alphaClippedState.BlendState = &m_blendStates.at("Transparent");
		alphaClippedState.RasterizerState = &m_rasterizerStates.at("NoCulling");

		m_pipelineStateObjects.emplace("AlphaClipped", alphaClippedState);
	}

	// Terrain:
	{
		PipelineState terrainState;
		terrainState.VertexShader = &m_vertexShaders.at("Terrain");
		terrainState.HullShader = &m_hullShaders.at("Terrain");
		terrainState.DomainShader = &m_domainShaders.at("Terrain");
		terrainState.PixelShader = &m_pixelShaders.at("Terrain");
		terrainState.RasterizerState = &m_rasterizerStates.at("Default");
		terrainState.BlendState = &m_blendStates.at("Default");
		terrainState.DepthStencilState = &m_depthStencilStates.at("Default");

		m_pipelineStateObjects.emplace("Terrain", terrainState);
	}

	// SkyDome:
	{
		PipelineState skydomeState;
		skydomeState.VertexShader = &m_vertexShaders.at("SkyDome");
		skydomeState.PixelShader = &m_pixelShaders.at("SkyDome");
		skydomeState.RasterizerState = &m_rasterizerStates.at("NoCulling");
		skydomeState.BlendState = &m_blendStates.at("Default");
		skydomeState.DepthStencilState = &m_depthStencilStates.at("DepthDisabled");
		m_pipelineStateObjects.emplace("SkyDome", skydomeState);
	}
}

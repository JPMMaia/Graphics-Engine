#include "stdafx.h"
#include "Graphics.h"

using namespace DirectX;
using namespace GraphicsEngine;

Graphics::Graphics(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight) :
	m_d3dBase(outputWindow, clientWidth, clientHeight),
	m_pipelineStateManager(m_d3dBase),
	m_camera(m_d3dBase.GetAspectRatio(), 0.25f * XM_PI, 1.0f, 1000.0f, XMMatrixIdentity()),
	m_scene()
{
	m_d3dBase.SetClearColor(XMFLOAT3(1.0f, 0.0f, 0.0f));
}

void Graphics::OnResize(uint32_t clientWidth, uint32_t clientHeight)
{
	// TODO
}

void Graphics::Update(const Common::Timer& timer)
{
	m_camera.Update();
}

void Graphics::Render(const Common::Timer& timer)
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	m_d3dBase.BeginScene();

	m_pipelineStateManager.SetPipelineState(deviceContext, "Standard");
	DrawRenderItems(deviceContext, RenderLayer::Opaque);

	m_d3dBase.EndScene();
}

Camera* Graphics::GetCamera()
{
	return &m_camera;
}

void Graphics::AddRenderItem(std::unique_ptr<RenderItem>&& renderItem, std::initializer_list<RenderLayer> renderLayers)
{
	for (auto renderLayer : renderLayers)
	{
		m_renderItemLayers[static_cast<SIZE_T>(renderLayer)].push_back(renderItem.get());
	}

	m_allRenderItems.push_back(std::move(renderItem));
}

void Graphics::DrawRenderItems(ID3D11DeviceContext* deviceContext, RenderLayer renderLayer) const
{
	// For each render item:
	for(auto& renderItem : m_renderItemLayers[static_cast<SIZE_T>(renderLayer)])
	{
		// TODO set cb
		renderItem->Render(deviceContext);
	}
}

#include "stdafx.h"
#include "Graphics.h"
#include "ShaderBufferTypes.h"

using namespace DirectX;
using namespace GraphicsEngine;

Graphics::Graphics(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight, bool fullscreen) :
	m_d3dBase(outputWindow, clientWidth, clientHeight, fullscreen),
	m_pipelineStateManager(m_d3dBase),
	m_camera(m_d3dBase.GetAspectRatio(), 0.25f * XM_PI, 1.0f, 1000.0f, XMMatrixIdentity()),
	m_scene(this, m_d3dBase),
	m_frameResources(1, FrameResource(m_d3dBase.GetDevice(), m_allRenderItems)),
	m_currentFrameResource(&m_frameResources[0])
{
	m_d3dBase.SetClearColor(XMFLOAT3(0.2f, 0.2f, 0.2f));
	m_camera.SetPosition(0.0f, 2.5f, -15.0f);
}

void Graphics::OnResize(uint32_t clientWidth, uint32_t clientHeight)
{
	// TODO
}

void Graphics::Update(const Common::Timer& timer)
{
	m_camera.Update();
	UpdateMatrixBuffers();
}

void Graphics::Render(const Common::Timer& timer) const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	m_d3dBase.BeginScene();

	m_pipelineStateManager.SetPipelineState(deviceContext, "Standard");
	DrawRenderItems(RenderLayer::Opaque);

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

void Graphics::UpdateMatrixBuffers()
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	ShaderBufferTypes::MatrixBuffer matrixBuffer;

	// Transpose and store view and projection matrices:
	XMStoreFloat4x4(&matrixBuffer.ViewMatrix, XMMatrixTranspose(m_camera.GetViewMatrix()));
	XMStoreFloat4x4(&matrixBuffer.ProjectionMatrix, XMMatrixTranspose(m_camera.GetProjectionMatrix()));

	for(auto& renderItem : m_allRenderItems)
	{
		// TODO optimize
	
		// Transpose and store world matrix:
		XMStoreFloat4x4(&matrixBuffer.WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&renderItem->WorldMatrix)));

		// Update buffer:
		m_currentFrameResource->MatrixBufferArray[renderItem->MatrixBufferIndex].Map(deviceContext, &matrixBuffer, sizeof(ShaderBufferTypes::MatrixBuffer));
	}
}

void Graphics::DrawRenderItems(RenderLayer renderLayer) const
{
	auto deviceContext = m_d3dBase.GetDeviceContext();

	// For each render item:
	for(auto& renderItem : m_renderItemLayers[static_cast<SIZE_T>(renderLayer)])
	{
		// Set matrix buffer:
		const auto& matrixBuffer = m_currentFrameResource->MatrixBufferArray[renderItem->MatrixBufferIndex];
		deviceContext->VSSetConstantBuffers(0, 1, matrixBuffer.GetAddressOf());
		
		// Render:
		renderItem->Render(deviceContext);
	}
}

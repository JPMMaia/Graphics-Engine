#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"

#include <Helpers.h>

using namespace Application;

using namespace DirectX;
using namespace std;
using namespace Windows::Foundation;
using namespace GraphicsEngine;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	const auto& outputSize = m_deviceResources->GetOutputSize();
	const auto& orientationMatrix = m_deviceResources->GetOrientationTransform3D();
	m_scene.CreateWindowSizeDependentResources(outputSize.Width, outputSize.Height, orientationMatrix);
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
}

void Sample3DSceneRenderer::HandleInput(const InputHandler& input)
{
	m_scene.HandleInput(input);
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
		return;

	auto context = m_deviceResources->GetD3DDeviceContext();

	m_scene.Render(context);
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	// Initialize scene:
	m_scene.CreateDeviceDependentResources(m_deviceResources->GetD3DDevice());

	// Once the scene is loaded, the object is ready to be rendered.
	m_loadingComplete = true;
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_scene.ReleaseDeviceDependentResources();
}
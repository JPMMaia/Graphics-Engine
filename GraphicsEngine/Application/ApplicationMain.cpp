#include "pch.h"
#include "ApplicationMain.h"
#include "Common\DirectXHelper.h"

using namespace Application;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;
using namespace GraphicsEngine;

// The DirectX 12 Application template is documented at http://go.microsoft.com/fwlink/?LinkID=613670&clcid=0x409

// Loads and initializes application assets when the application is loaded.
ApplicationMain::ApplicationMain()
{
}

// Creates and initializes the renderers.
void ApplicationMain::CreateRenderers(const std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	m_sceneRenderer = std::unique_ptr<SceneRenderer>(new SceneRenderer());

	OnWindowSizeChanged();
}

void ApplicationMain::Update(const Timer& timer)
{
	m_sceneRenderer->Update(timer);
}
bool ApplicationMain::Render(const Timer& timer)
{
	m_sceneRenderer->Render(timer);

	return true;
}

// Updates application state when the window's size changes (e.g. device orientation change)
void ApplicationMain::OnWindowSizeChanged()
{
	m_sceneRenderer->CreateWindowSizeDependentResources();
}

// Notifies the app that it is being suspended.
void ApplicationMain::OnSuspending()
{
	// Process lifetime management may terminate suspended apps at any time, so it is
	// good practice to save any state that will allow the app to restart where it left off.

	m_sceneRenderer->SaveState();

	// If your application uses video memory allocations that are easy to re-create,
	// consider releasing that memory to make it available to other applications.
}

// Notifes the app that it is no longer suspended.
void ApplicationMain::OnResuming()
{
}

// Notifies renderers that device resources need to be released.
void ApplicationMain::OnDeviceRemoved()
{
	// TODO: Save any necessary application or renderer state and release the renderer
	// and its resources which are no longer valid.
	m_sceneRenderer->SaveState();
	m_sceneRenderer = nullptr;
}

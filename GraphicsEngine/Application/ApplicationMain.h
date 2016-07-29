#pragma once

#include "Common\DeviceResources.h"
#include "Content\Sample3DSceneRenderer.h"
#include <Content\SceneRenderer.h>
#include <Content\Timer.h>

namespace Application
{
	class ApplicationMain
	{
	public:
		ApplicationMain();
		void CreateRenderers(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void Update(const GraphicsEngine::Timer& timer);
		bool Render(const GraphicsEngine::Timer& timer);

		void OnWindowSizeChanged();
		void OnSuspending();
		void OnResuming();
		void OnDeviceRemoved();

	private:
		std::unique_ptr<GraphicsEngine::SceneRenderer> m_sceneRenderer;
	};
}
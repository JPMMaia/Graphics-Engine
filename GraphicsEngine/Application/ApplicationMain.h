#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\Sample3DSceneRenderer.h"
#include "Content\SampleFpsTextRenderer.h"

#include <InputHandler.h>

// Renders Direct2D and 3D content on the screen.
namespace Application
{
	class ApplicationMain : public DX::IDeviceNotify
	{
	public:
		ApplicationMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~ApplicationMain();
		void CreateWindowSizeDependentResources();
		void Update();
		bool Render();

		void OnKeyDown(Windows::System::VirtualKey key);
		void OnKeyUp(Windows::System::VirtualKey key);

		// IDeviceNotify
		virtual void OnDeviceLost() override;
		virtual void OnDeviceRestored() override;

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// TODO: Replace with your own content renderers.
		std::unique_ptr<Sample3DSceneRenderer> m_sceneRenderer;
		std::unique_ptr<SampleFpsTextRenderer> m_fpsTextRenderer;

		// Rendering loop timer.
		DX::StepTimer m_timer;

		GraphicsEngine::InputHandler m_input;
	};
}
#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"

#include <vector>
#include <string>

#include <Scene.h>
#include <InputHandler.h>

namespace Application
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		void* operator new(size_t size);
		void operator delete(void* pointer);

		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void HandleInput(const GraphicsEngine::InputHandler& input);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;

		GraphicsEngine::Scene m_scene;
	};
}


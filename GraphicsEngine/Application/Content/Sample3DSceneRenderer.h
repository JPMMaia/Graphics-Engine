#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"

#include <IndexBuffer.h>
#include <VertexBuffer.h>
#include <VertexShader.h>
#include <PixelShader.h>
#include <ConstantBuffer.h>
#include <Scene.h>

namespace Application
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }


	private:
		void Rotate(float radians);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		GraphicsEngine::VertexBuffer<VertexPositionColor>					m_vertexBuffer;
		GraphicsEngine::IndexBuffer<uint16_t>								m_indexBuffer;
		GraphicsEngine::VertexShader										m_vertexShader;
		GraphicsEngine::PixelShader											m_pixelShader;
		GraphicsEngine::ConstantBuffer<ModelViewProjectionConstantBuffer>	m_constantBuffer;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		uint32	m_indexCount;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
	};
}


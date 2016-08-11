#pragma once

#include "D3DBase.h"
#include "Technique.h"
#include "Timer.h"

namespace GraphicsEngine
{
	class Graphics
	{
	public:
		explicit Graphics(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight);

		void OnResize(uint32_t clientWidth, uint32_t clientHeight);
		void Update(const Timer& timer);
		void Render(const Timer& timer);
		
	private:
		void InitializeGeometry(const D3DBase& d3dBase);

	private:
		D3DBase m_d3d;
		Technique m_technique;
		
		std::unique_ptr<MeshGeometry> m_boxGeometry;
		
		DirectX::XMFLOAT4X4 m_modelMatrix;
		DirectX::XMFLOAT4X4 m_viewMatrix;
		DirectX::XMFLOAT4X4 m_projectionMatrix;
	};
}

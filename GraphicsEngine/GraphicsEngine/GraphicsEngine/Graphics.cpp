#include "stdafx.h"
#include "Graphics.h"

using namespace DirectX;
using namespace GraphicsEngine;

Graphics::Graphics(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight) :
	m_d3dBase(outputWindow, clientWidth, clientHeight)
{
	m_d3dBase.SetClearColor(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f));
}

void Graphics::OnResize(uint32_t clientWidth, uint32_t clientHeight)
{
	// TODO
}

void Graphics::Update(const Common::Timer& timer)
{
	// TODO
}

void Graphics::Render(const Common::Timer& timer)
{
	m_d3dBase.BeginScene();

	// TODO

	m_d3dBase.EndScene();
}

Camera* Graphics::GetCamera()
{
	return &m_camera;
}

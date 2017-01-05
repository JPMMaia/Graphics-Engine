#include "Application.h"

using namespace Common;
using namespace Win32Application;
using namespace GraphicsEngine;
using namespace DirectX;

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// Check if the window is being destroyed:
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// Check if the window is being closed:
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

		// All other messages pass to the message handler in the application class:
	default:
		auto application = Application::GetInstance();
		if (application)
			return application->MessageHandler(hwnd, msg, wParam, lParam);
		
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

Application* Application::GetInstance()
{
	if (!s_instance)
	{
		if (!s_mutex.try_lock())
			return nullptr;

		s_instance = std::make_unique<Application>();

		s_mutex.unlock();
	}

	return s_instance.get();
}

int Application::Run()
{
	MSG message = {};

	auto update = [this](const Timer& timer)
	{
		auto camera = m_graphics.GetCamera();
		auto scalar = 0.1f *  static_cast<float>(timer.GetMillisecondsPerUpdate());
		if (m_input.IsKeyDown(DIK_W))
			camera->MoveForward(scalar);
		if (m_input.IsKeyDown(DIK_S))
			camera->MoveForward(-scalar);
		if (m_input.IsKeyDown(DIK_A))
			camera->MoveRight(-scalar);
		if (m_input.IsKeyDown(DIK_D))
			camera->MoveRight(scalar);

		static const auto mouseSensibility = 0.005f;
		int mouseDeltaX, mouseDeltaY;
		m_input.GetMouseVelocity(mouseDeltaX, mouseDeltaY);
		camera->RotateWorldY(mouseDeltaX * mouseSensibility);
		camera->RotateLocalX(mouseDeltaY * mouseSensibility);

		// Clamp camera to the ground:
		/*XMFLOAT3 position;
		XMStoreFloat3(&position, camera->GetPosition());
		auto terrainHeight = m_graphics.GetScene()->GetTerrain().GetTerrainHeight(position.x, position.z);
		camera->SetPosition(position.x, 5.0f + terrainHeight, position.z);*/

		m_graphics.FixedUpdate(m_timer);
	};

	auto render = [this](const Timer& timer)
	{
		m_graphics.RenderUpdate(timer);
		m_graphics.Render(timer);
	};

	auto processInput = [this]()
	{
		m_input.Update();

		// Exit application if exit button is pressed:
		if (m_input.IsKeyDown(DIK_ESCAPE))
			return false;

		if (m_input.IsKeyDown(DIK_T))
			m_soundManager.Play2DSound("TestSound", 1.0f);

		return true;
	};

	auto processFrameStatistics = [this](const Timer& timer)
	{
		auto extraCaption = L"FPS: " + std::to_wstring(timer.GetFramesPerSecond()) + L" | MSPF: " + std::to_wstring(timer.GetMillisecondsPerFrame()) + L" | V: " + std::to_wstring(m_graphics.GetVisibleInstances());
		m_window.SetWindowExtraCaption(extraCaption);
	};

	m_timer.Reset();

	while (message.message != WM_QUIT)
	{
		// If there are Window messages then process them:
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		// Otherwise, do animation/game stuff:
		else if (!m_timer.UpdateAndRender(update, render, processInput, processFrameStatistics))
		{
			break;
		}
	}

	return static_cast<int>(message.wParam);
}

LRESULT Application::MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

std::mutex Application::s_mutex;
std::unique_ptr<Application> Application::s_instance;
Application::Application() :
	m_window(MainWindowProc),
	m_timer(c_millisecondsPerUpdate),
	m_input(m_window.GetHInstance(), m_window.GetWindowHandle(), m_window.GetClientWidth(), m_window.GetClientHeight()),
	m_graphics(m_window.GetWindowHandle(), m_window.GetClientWidth(), m_window.GetClientHeight(), m_window.IsFullscreen()),
	m_soundManager(m_window.GetWindowHandle())
{
	m_soundManager.Create2DSoundFromWaveFile("TestSound", L"Sounds/Sound01.wav");
}

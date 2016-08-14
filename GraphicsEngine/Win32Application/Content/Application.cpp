#include "Application.h"

using namespace Win32Application;
using namespace GraphicsEngine;
using namespace DirectX;

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// Check if the window is being closed:
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

		// All other messages pass to the message handler in the application class:
	default:
		return Application::GetInstance().MessageHandler(hwnd, msg, wParam, lParam);
	}
}

Application& Application::GetInstance()
{
	return s_instance;
}

int Application::Run()
{
	MSG message = {};

	auto update = [this](const Timer& timer)
	{
	};

	auto render = [this](const Timer& timer)
	{
		m_graphics.Update(timer);
		m_graphics.Render(timer);
	};

	auto processInput = [this]()
	{
		m_input.Frame();

		// Exit application if exit function is pressed:
		if (m_input.IsKeyDown(DIK_ESCAPE))
			return false;

		if (m_input.IsKeyDown(DIK_1))
			m_graphics.SetWireframeMode(false);
		else if (m_input.IsKeyDown(DIK_2))
			m_graphics.SetWireframeMode(true);

		auto camera = m_graphics.GetCamera();
		static const auto scalar = 0.01f;
		if (m_input.IsKeyDown(DIK_W))
			camera->MoveForward(scalar);
		if (m_input.IsKeyDown(DIK_S))
			camera->MoveForward(-scalar);
		if (m_input.IsKeyDown(DIK_A))
			camera->MoveLeft(scalar);
		if (m_input.IsKeyDown(DIK_D))
			camera->MoveLeft(-scalar);
		
		static const auto mouseSensibility = 0.005f;
		int mouseDeltaX, mouseDeltaY;
		m_input.GetMouseVelocity(mouseDeltaX, mouseDeltaY);
		camera->RotateWorldY(-mouseDeltaX * mouseSensibility);
		camera->RotateLocalX(mouseDeltaY * mouseSensibility);

		return true;
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
		else if (!m_timer.UpdateAndRender(update, render, processInput))
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

Application Application::s_instance = Application();
Application::Application() :
	m_window(MainWindowProc),
	m_timer(c_millisecondsPerUpdate),
	m_graphics(m_window.GetWindowHandle(), m_window.GetClientWidth(), m_window.GetClientHeight()),
	m_input(m_window.GetHInstance(), m_window.GetWindowHandle(), m_window.GetClientWidth(), m_window.GetClientHeight())
{
}

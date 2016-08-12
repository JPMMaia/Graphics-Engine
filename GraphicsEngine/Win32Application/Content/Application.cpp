#include "Application.h"

using namespace Win32Application;
using namespace GraphicsEngine;

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
		m_graphics.Update(timer);
	};

	auto render = [this](const Timer& timer)
	{
		m_graphics.Render(timer);
	};

	auto processInput = [this]()
	{
		// Exit application if exit function is pressed:
		if (m_input.IsKeyDown(VK_ESCAPE))
			return false;

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
	switch (msg)
	{
		// Check if a key has been pressed on the keyboard:
	case WM_KEYDOWN:

		// If a key is pressed send it to the input object so it can record that state:
		m_input.OnKeyDown(wParam);
		return 0;

		// Check if a key has been released on the keyboard:
	case WM_KEYUP:

		// If a key is released then send it to the input object so it can unset the state for that key:
		m_input.OnKeyUp(wParam);
		return 0;

		// Any other messages send to the default message handler as our application won't make use of them:
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

Application Application::s_instance = Application();
Application::Application() :
	m_window(MainWindowProc),
	m_timer(c_millisecondsPerUpdate),
	m_graphics(m_window.GetWindowHandle(), m_window.GetClientWidth(), m_window.GetClientHeight())
{
}

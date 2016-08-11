#pragma once

#include "Content/Graphics.h"
#include "Content/InputHandler.h"
#include "Content/Timer.h"
#include "Window.h"

namespace Win32Application
{
	class Application
	{
	public:
		static Application& GetInstance();

		int Run();

		LRESULT CALLBACK MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	protected:
		Application();

	private:
		static Application s_instance;
		Window m_window;

		const double c_millisecondsPerUpdate = 10.0;
		GraphicsEngine::Timer m_timer;

		GraphicsEngine::InputHandler m_input;
		GraphicsEngine::Graphics m_graphics;
	};
}

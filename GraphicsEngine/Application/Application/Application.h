#pragma once

#include "GraphicsEngine/Graphics.h"
#include "GraphicsEngine/DXInputHandler.h"
#include "GraphicsEngine/Timer.h"
#include "SoundEngine/SoundManager.h"
#include "Window.h"

namespace Win32Application
{
	class Application
	{
	public:
		static Application& GetInstance();

		int Run();

		static LRESULT CALLBACK MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	protected:
		Application();

	private:
		static Application s_instance;
		Window m_window;

		const double c_millisecondsPerUpdate = 10.0;
		GraphicsEngine::Timer m_timer;

		GraphicsEngine::DXInputHandler m_input;
		GraphicsEngine::Graphics m_graphics;
		SoundEngine::SoundManager m_soundManager;
	};
}

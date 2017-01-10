#pragma once

#include "GraphicsEngine/Graphics.h"
#include "GraphicsEngine/DXInputHandler.h"
#include "Common/Timer.h"
#include "SoundEngine/SoundManager.h"
#include "Window.h"
#include <mutex>
#include <random>
#include <stack>

namespace Win32Application
{
	class Application
	{
	public:
		static Application* GetInstance();
		static LRESULT CALLBACK MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	public:
		Application();

		int Run();

		void OnKeyboardKeyDown(void* sender, const GraphicsEngine::DXInputHandler::KeyboardEventArgs& eventArgs);

	private:
		static std::mutex s_mutex;
		static std::unique_ptr<Application> s_instance;
		Window m_window;

		const double c_millisecondsPerUpdate = 10.0;
		Common::Timer m_timer;

		GraphicsEngine::DXInputHandler m_input;
		GraphicsEngine::Graphics m_graphics;
		SoundEngine::SoundManager m_soundManager;

		std::random_device m_randomDevice;
		std::default_random_engine m_randomEngine;
		std::uniform_real_distribution<float> m_randomAngles;
		std::uniform_real_distribution<float> m_randomScales;
	};
}

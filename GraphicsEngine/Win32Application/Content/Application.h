#pragma once

#include "Window.h"

#include <memory>

namespace Win32Application
{
	class Application
	{
	public:
		static Application* GetInstance();

	protected:
		Application();

	private:
		static std::unique_ptr<Application> s_instance;
		Window m_window;
	};
}

#pragma once

#include <Windows.h>
#include <cstdint>
#include <string>

namespace Win32Application
{
	class Window
	{
	public:
		explicit Window(WNDPROC mainWindowProc);
		~Window();

		HWND GetWindowHandle() const;
		uint32_t GetClientWidth() const;
		uint32_t GetClientHeight() const;

	protected:
		bool Initialize(WNDPROC mainWindowProc);
		void Shutdown();

	protected:
		HINSTANCE m_hInstance = nullptr;
		HWND m_windowHandle = nullptr;

		std::wstring m_applicationName = L"DirectX12Application";
		std::wstring m_windowCaption = L"DirectX 12 Application";
		bool m_fullscreen = false;
		uint32_t m_clientWidth = 800;
		uint32_t m_clientHeight = 600;
	};
}

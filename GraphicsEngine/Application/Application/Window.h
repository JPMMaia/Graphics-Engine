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

		HINSTANCE GetHInstance() const;
		HWND GetWindowHandle() const;
		uint32_t GetClientWidth() const;
		uint32_t GetClientHeight() const;
		bool IsFullscreen() const;

		void SetWindowExtraCaption(const std::wstring& extraCaption) const;

	protected:
		bool Initialize(WNDPROC mainWindowProc);
		void Shutdown();

	protected:
		HINSTANCE m_hInstance = nullptr;
		HWND m_windowHandle = nullptr;

		std::wstring m_applicationName = L"DirectX11Application";
		std::wstring m_windowCaption = L"DirectX 11 Application";
		bool m_fullscreen;
		uint32_t m_clientWidth = 800;
		uint32_t m_clientHeight = 600;
	};
}

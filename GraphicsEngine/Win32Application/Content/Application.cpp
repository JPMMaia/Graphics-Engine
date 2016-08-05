#include "Application.h"

using namespace Win32Application;

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//return D3DApp::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
	return LRESULT();
}

Application* Application::GetInstance()
{
	if (s_instance.get() == nullptr)
		s_instance = std::make_unique<Application>();

	return s_instance.get();
}

Application::Application() :
	m_window(MainWindowProc)
{
}

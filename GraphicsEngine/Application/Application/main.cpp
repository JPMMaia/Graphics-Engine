#include "Application.h"

using namespace Win32Application;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application::GetInstance()->Run();

	return 0;
}
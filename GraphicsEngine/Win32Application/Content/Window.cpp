#include "Window.h"

using namespace Win32Application;

Window::Window(WNDPROC mainWindowProc)
{
	if (!Initialize(mainWindowProc))
		throw std::exception("Failed to create window.");
}
Window::~Window()
{
	Shutdown();
}

bool Window::Initialize(WNDPROC mainWindowProc)
{
	// Get the instance of this application:
	m_hInstance = GetModuleHandle(nullptr);

	// Setup the windows class with default settings:
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = mainWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = m_applicationName.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class:
	if (!RegisterClassEx(&wc))
	{
		MessageBox(nullptr, L"RegisterClass Failed.", nullptr, 0);
		return false;
	}

	// Determine the resolution of the clients desktop screen:
	auto screenHeight = GetSystemMetrics(SM_CYSCREEN);
	auto screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int positionX, positionY;

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode:
	if (m_fullscreen)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit:
		DEVMODE dmScreenSettings = {};
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsHeight = static_cast<unsigned long>(screenHeight);
		dmScreenSettings.dmPelsWidth = static_cast<unsigned long>(screenWidth);
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen:
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner:
		positionX = positionY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution:
		screenWidth = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		positionX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		positionY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it:
	m_hWindow = CreateWindowEx(
		WS_EX_APPWINDOW, 
		m_applicationName.c_str(), 
		m_applicationName.c_str(),
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		positionX, positionY, screenWidth, screenHeight,
		nullptr, 
		nullptr, 
		m_hInstance, nullptr
		);

	if (!m_hWindow)
	{
		MessageBox(nullptr, L"CreateWindow Failed.", nullptr, 0);
		return false;
	}

	// Bring the window up on the screen and set it as main focus:
	ShowWindow(m_hWindow, SW_SHOW);
	SetForegroundWindow(m_hWindow);
	SetFocus(m_hWindow);

	// Hide the mouse cursor:
	ShowCursor(false);

	return true;
}

void Window::Shutdown()
{
	// Show the mouse cursor:
	ShowCursor(true);

	// Fix display settings if leaving full screen mode:
	if (m_fullscreen)
		ChangeDisplaySettings(nullptr, 0);

	// Remove the window:
	DestroyWindow(m_hWindow);
	m_hWindow = nullptr;

	// Remove the application instance:
	UnregisterClass(m_applicationName.c_str(), m_hInstance);
	m_hInstance = nullptr;
}

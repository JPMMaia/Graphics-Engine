#include "stdafx.h"
#include "DXInputHandler.h"
#include "Common/Helpers.h"

using namespace Common;
using namespace GraphicsEngine;
using namespace Microsoft::WRL;

DXInputHandler::DXInputHandler(HINSTANCE hInstance, HWND hWindow, uint32_t clientWidth, uint32_t clientHeight)
{
	// Store the screen size which will be used for positioning the mouse cursor:
	m_screenWidth = clientWidth;
	m_screenHeight = clientHeight;

	// Initialize the location of the mouse on the screen:
	m_mousePositionX = 0;
	m_mousePositionY = 0;

	// Initialize the main direct input interface:
	ThrowIfFailed(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(m_directInput.GetAddressOf()), nullptr));

	// Initialize the direct input interface for the keyboard:
	ThrowIfFailed(m_directInput->CreateDevice(GUID_SysKeyboard, m_keyboard.GetAddressOf(), nullptr));

	// Set the data format. In this case since it is a keyboard we can use the predefined data format.
	ThrowIfFailed(m_keyboard->SetDataFormat(&c_dfDIKeyboard));

	// Set the cooperative level of the keyboard to not share with other programs:
	ThrowIfFailed(m_keyboard->SetCooperativeLevel(hWindow, DISCL_FOREGROUND | DISCL_EXCLUSIVE));

	// Now acquire the keyboard.
	ThrowIfFailed(m_keyboard->Acquire());

	// Initialize the direct input interface for the mouse.
	ThrowIfFailed(m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, nullptr));

	// Set the data format for the mouse using the pre-defined mouse data format.
	ThrowIfFailed(m_mouse->SetDataFormat(&c_dfDIMouse2));

	// Set the cooperative level of the mouse to share with other programs.
	ThrowIfFailed(m_mouse->SetCooperativeLevel(hWindow, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));

	// Acquire the mouse.
	ThrowIfFailed(m_mouse->Acquire());
}
DXInputHandler::~DXInputHandler()
{
	// Release the mouse:
	if (m_mouse)
		m_mouse->Unacquire();

	// Release the keyboard.
	if (m_keyboard)
		m_keyboard->Unacquire();
}

void DXInputHandler::Update()
{
	// Read the current state of the keyboard:
	ReadKeyboard();

	// Read the current state of the mouse:
	ReadMouse();

	// Process the changes in the mouse and keyboard:
	ProcessInput();
}

void DXInputHandler::ReadKeyboard()
{
	m_previousKeyboardState.swap(m_currentKeyboardState);

	// Read the keyboard device:
	auto result = m_keyboard->GetDeviceState(static_cast<DWORD>(m_currentKeyboardState.size() * sizeof(uint8_t)), m_currentKeyboardState.data());
	if (FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back:
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			ThrowIfFailed(result);
		}
	}
}

void DXInputHandler::ReadMouse()
{
	// Read the mouse device:
	auto result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &m_mouseState);
	if (FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back:
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			ThrowIfFailed(result);
		}
	}
}
void DXInputHandler::ProcessInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame:
	m_mousePositionX += m_mouseState.lX;
	m_mousePositionY += m_mouseState.lY;

	// Ensure the mouse location doesn't exceed the screen width or height:
	if (m_mousePositionX < 0) m_mousePositionX = 0;
	if (m_mousePositionY < 0) m_mousePositionY = 0;
	if (m_mousePositionX > static_cast<int>(m_screenWidth)) m_mousePositionX = m_screenWidth;
	if (m_mousePositionY > static_cast<int>(m_screenHeight)) m_mousePositionY = m_screenHeight;

	for(const auto& event : m_onKeyboardKeyDownEvents)
	{
		auto key = event.first;

		if (!(m_previousKeyboardState[key] & 0x80) && (m_currentKeyboardState[key] & 0x80))
		{
			event.second.Raise(this, KeyboardEventArgs(key));
		}	
	}
}

void DXInputHandler::GetMousePosition(uint32_t& mouseX, uint32_t& mouseY) const
{
	mouseX = static_cast<uint32_t>(m_mousePositionX);
	mouseY = static_cast<uint32_t>(m_mousePositionY);
}

void DXInputHandler::GetMouseVelocity(int& deltaX, int& deltaY) const
{
	deltaX = m_mouseState.lX;
	deltaY = m_mouseState.lY;
}

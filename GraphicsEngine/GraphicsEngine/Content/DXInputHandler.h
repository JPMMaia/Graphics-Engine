#pragma once

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>
#include <cstdint>

namespace GraphicsEngine
{
	class DXInputHandler
	{
	public:
		DXInputHandler(HINSTANCE hInstance, HWND hWindow, uint32_t clientWidth, uint32_t clientHeight);
		~DXInputHandler();

		void Frame();

		template<typename VirtualKey, typename = std::enable_if_t<std::is_enum<VirtualKey>::value || std::is_integral<VirtualKey>::value>>
		bool IsKeyDown(VirtualKey key) const;

		void GetMousePosition(uint32_t& mouseX, uint32_t& mouseY) const;
		void GetMouseVelocity(int& deltaX, int& deltaY) const;

	private:
		void ReadKeyboard();
		void ReadMouse();
		void ProcessInput();

	private:
		Microsoft::WRL::ComPtr<IDirectInput8> m_directInput;
		Microsoft::WRL::ComPtr<IDirectInputDevice8> m_keyboard;
		Microsoft::WRL::ComPtr<IDirectInputDevice8> m_mouse;

		uint8_t m_keyboardState[256];
		DIMOUSESTATE2 m_mouseState;

		uint32_t m_screenWidth, m_screenHeight;
		int m_mousePositionX, m_mousePositionY;
	};

	template <typename VirtualKey, typename>
	bool DXInputHandler::IsKeyDown(VirtualKey key) const
	{
		// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed:
		if (m_keyboardState[key] & 0x80)
			return true;

		return false;
	}
}

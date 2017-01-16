#pragma once

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

// ReSharper disable once CppUnusedIncludeDirective
#include <cstdint>

#include <dinput.h>
#include <wrl/client.h>
#include "Common/Event.h"
#include <unordered_map>

namespace GraphicsEngine
{
	class DXInputHandler
	{
	public:
		struct KeyboardEventArgs
		{
			uint8_t Key;
			bool UserInput;

			explicit KeyboardEventArgs(uint8_t key, bool userInput) :
				Key(key),
				UserInput(userInput)
			{
			}
		};

	public:
		DXInputHandler(HINSTANCE hInstance, HWND hWindow, uint32_t clientWidth, uint32_t clientHeight);
		~DXInputHandler();

		void Update();

		template<typename VirtualKey, typename = std::enable_if_t<std::is_enum<VirtualKey>::value || std::is_integral<VirtualKey>::value>>
		bool IsKeyDown(VirtualKey key) const
		{
			// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed:
			if (m_currentKeyboardState[key] & 0x80)
				return true;

			return false;
		}

		void GetMousePosition(uint32_t& mouseX, uint32_t& mouseY) const;
		void GetMouseVelocity(int& deltaX, int& deltaY) const;

		template<typename VirtualKey, typename = std::enable_if_t<std::is_enum<VirtualKey>::value || std::is_integral<VirtualKey>::value>>
		void SubscribeToOnKeyDownEvents(VirtualKey key, const  Common::Event<KeyboardEventArgs>::EventFunctionType& onKeyDown)
		{
			auto location = m_onKeyboardKeyDownEvents.find(key);
			if (location == m_onKeyboardKeyDownEvents.end())
			{
				auto event = Common::Event<KeyboardEventArgs>();
				event.Subscribe(onKeyDown);
				m_onKeyboardKeyDownEvents.emplace(key, event);
				return;
			}

			location->second.Subscribe(onKeyDown);
		}

		template<typename VirtualKey, typename = std::enable_if_t<std::is_enum<VirtualKey>::value || std::is_integral<VirtualKey>::value>>
		void PressKey(VirtualKey key) const
		{
			auto keyValue = static_cast<uint8_t>(key);
			const auto& event = m_onKeyboardKeyDownEvents.at(keyValue);
			event.Raise(this, KeyboardEventArgs(keyValue, false));
		}

	private:
		void ReadKeyboard();
		void ReadMouse();
		void ProcessInput();

	private:
		Microsoft::WRL::ComPtr<IDirectInput8> m_directInput;
		Microsoft::WRL::ComPtr<IDirectInputDevice8> m_keyboard;
		Microsoft::WRL::ComPtr<IDirectInputDevice8> m_mouse;

		static constexpr size_t s_keyCount = 256;
		std::array<uint8_t, s_keyCount> m_currentKeyboardState;
		std::array<uint8_t, s_keyCount> m_previousKeyboardState;
		DIMOUSESTATE2 m_mouseState;

		uint32_t m_screenWidth, m_screenHeight;
		int m_mousePositionX, m_mousePositionY;

		std::unordered_map<uint8_t, Common::Event<KeyboardEventArgs>> m_onKeyboardKeyDownEvents;
	};
}

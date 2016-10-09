#pragma once

#include <array>

namespace GraphicsEngine
{
	class InputHandler
	{
	public:
		InputHandler();

		template<typename VirtualKey, typename = std::enable_if_t<std::is_enum<VirtualKey>::value || std::is_integral<VirtualKey>::value>>
		void OnKeyDown(VirtualKey key);

		template<typename VirtualKey, typename = std::enable_if_t<std::is_enum<VirtualKey>::value || std::is_integral<VirtualKey>::value>>
		void OnKeyUp(VirtualKey key);

		template<typename VirtualKey, typename = std::enable_if_t<std::is_enum<VirtualKey>::value || std::is_integral<VirtualKey>::value>>
		bool IsKeyDown(VirtualKey key) const;

	private:
		std::array<bool, 256> m_keys;
	};

	template<typename VirtualKey, typename>
	void InputHandler::OnKeyDown(VirtualKey key)
	{
		m_keys[static_cast<uint32_t>(key)] = true;
	}

	template<typename VirtualKey, typename>
	void InputHandler::OnKeyUp(VirtualKey key)
	{
		m_keys[static_cast<uint32_t>(key)] = false;
	}

	template<typename VirtualKey, typename>
	bool InputHandler::IsKeyDown(VirtualKey key) const
	{
		return m_keys[static_cast<uint32_t>(key)];
	}
}
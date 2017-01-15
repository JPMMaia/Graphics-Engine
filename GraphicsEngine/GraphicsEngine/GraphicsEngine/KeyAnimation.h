#pragma once

#include "BaseAnimation.h"


namespace GraphicsEngine
{
	class DXInputHandler;

	class KeyAnimation : public BaseAnimation
	{
	public:
		explicit KeyAnimation(DXInputHandler& inputHandler);
		KeyAnimation(DXInputHandler& inputHandler, uint8_t key, float startInMilliseconds);

		void FixedUpdate(const Common::Timer& timer) const override;
		
		uint8_t GetKey() const;
		void SetKey(uint8_t key);

		nlohmann::json ToJson() const override;
		static KeyAnimation FromJson(const nlohmann::json& objectJson, DXInputHandler& inputHandler);

	private:
		DXInputHandler& m_inputHandler;
		uint8_t m_key;
	};
}

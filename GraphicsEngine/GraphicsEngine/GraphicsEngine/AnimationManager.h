#pragma once

#include "Common/Timer.h"
#include "BaseAnimation.h"
#include "CameraAnimation.h"

namespace GraphicsEngine
{
	class Graphics;
	class DXInputHandler;

	class AnimationManager
	{
	public:
		AnimationManager(Graphics& graphics, DXInputHandler& inputHandler, const std::wstring& filename);
		~AnimationManager();

		void FixedUpdate(const Common::Timer& timer);

		void AddAnimation(std::unique_ptr<BaseAnimation>&& pAnimation, bool runNow);
		void AddAnimation(std::unique_ptr<CameraAnimation>&& pAnimation);
		DirectX::XMVECTOR GetLastCameraPosition() const;
		DirectX::XMVECTOR GetLastCameraRotationQuaternion() const;
		float GetLastCameraFinalTime() const;

		void SaveToFile() const;
		void LoadFromFile();

	private:
		Graphics& m_graphics;
		DXInputHandler& m_inputHandler;
		std::wstring m_filename;
		std::vector<std::unique_ptr<BaseAnimation>> m_allAnimations;
		std::vector<BaseAnimation*> m_animationsToRun;
		DirectX::XMVECTOR m_lastCameraPosition;
		DirectX::XMVECTOR m_lastCameraRotationQuaternion;
		float m_lastCameraFinalTime;
	};
}

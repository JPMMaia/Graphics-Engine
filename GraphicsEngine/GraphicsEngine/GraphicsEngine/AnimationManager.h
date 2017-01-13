#pragma once

#include "Common/Timer.h"
#include "BaseAnimation.h"
#include "CameraAnimation.h"

namespace GraphicsEngine
{
	class Graphics;

	class AnimationManager
	{
	public:
		explicit AnimationManager(Graphics& graphics, const std::wstring& filename);
		~AnimationManager();

		void FixedUpdate(const Common::Timer& timer);

		void AddAnimation(std::unique_ptr<BaseAnimation>&& pAnimation);
		void AddAnimation(std::unique_ptr<CameraAnimation>&& pAnimation);
		DirectX::XMVECTOR GetLastCameraPosition() const;
		DirectX::XMVECTOR GetLastCameraRotationQuaternion() const;
		float GetLastCameraFinalTime() const;

		void SaveToFile() const;
		void LoadFromFile();

	private:
		Graphics& m_graphics;
		std::wstring m_filename;
		std::vector<std::unique_ptr<BaseAnimation>> m_animations;
		DirectX::XMVECTOR m_lastCameraPosition;
		DirectX::XMVECTOR m_lastCameraRotationQuaternion;
		float m_lastCameraFinalTime;
	};
}

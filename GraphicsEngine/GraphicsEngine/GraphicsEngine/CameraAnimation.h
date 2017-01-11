#pragma once

#include "BaseAnimation.h"

#include <DirectXMath.h>

namespace GraphicsEngine
{
	class Camera;

	class CameraAnimation : public BaseAnimation
	{
	public:
		CameraAnimation(Camera& camera, float startInMilliseconds, float durationInMilliseconds, DirectX::CXMVECTOR initialPosition, DirectX::CXMVECTOR finalPosition, DirectX::CXMVECTOR initialRotationQuaternion, DirectX::CXMVECTOR finalRotationQuaternion);

		void FixedUpdate(const Common::Timer& timer) const override;

	private:
		Camera& m_camera;
		DirectX::XMVECTOR m_initialPosition;
		DirectX::XMVECTOR m_finalPosition;
		DirectX::XMVECTOR m_initialRotationQuaternion;
		DirectX::XMVECTOR m_finalRotationQuaternion;
	};
}

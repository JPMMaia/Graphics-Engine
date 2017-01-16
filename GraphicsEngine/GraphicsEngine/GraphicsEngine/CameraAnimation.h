#pragma once

#include "BaseAnimation.h"
#include "Camera.h"

#include <DirectXMath.h>

namespace GraphicsEngine
{
	class CameraAnimation : public BaseAnimation
	{
	public:

		explicit CameraAnimation(Camera& camera);

		CameraAnimation(Camera& camera, float startInMilliseconds, float durationInMilliseconds, DirectX::CXMVECTOR initialPosition, DirectX::CXMVECTOR finalPosition, DirectX::CXMVECTOR initialRotationQuaternion, DirectX::CXMVECTOR finalRotationQuaternion);

		void FixedUpdate(const Common::Timer& timer) const override;

		nlohmann::json ToJson() const override;
		static CameraAnimation FromJson(const nlohmann::json& objectJson, Camera& camera);

		DirectX::XMVECTOR GetInitialPosition() const;
		void SetInitialPosition(const DirectX::XMVECTOR& initialPosition);
		DirectX::XMVECTOR GetFinalPosition() const;
		void SetFinalPosition(const DirectX::XMVECTOR& finalPosition);
		DirectX::XMVECTOR GetInitialRotationQuaternion() const;
		void SetInitialRotationQuaternion(const DirectX::XMVECTOR& initialRotationQuaternion);
		DirectX::XMVECTOR GetFinalRotationQuaternion() const;
		void SetFinalRotationQuaternion(const DirectX::XMVECTOR& finalRotationQuaternion);

	private:
		Camera& m_camera;
		DirectX::XMVECTOR m_initialPosition;
		DirectX::XMVECTOR m_finalPosition;
		DirectX::XMVECTOR m_initialRotationQuaternion;
		DirectX::XMVECTOR m_finalRotationQuaternion;
	};
}

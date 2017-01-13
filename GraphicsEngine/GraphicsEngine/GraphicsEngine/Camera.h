#pragma once

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include "Ray.h"

namespace GraphicsEngine
{
	__declspec(align(16))
	class Camera
	{
	public:
		Camera();
		Camera(float aspectRatio, float fovAngleY, float nearZ, float farZ, DirectX::CXMMATRIX orientationMatrix);

		void Update();

		void XM_CALLCONV Move(DirectX::FXMVECTOR axis, float scalar);
		void MoveRight(float scalar);
		void MoveForward(float scalar);

		void XM_CALLCONV Rotate(DirectX::FXMVECTOR axis, float radians);
		void RotateLocalX(float radians);
		void RotateLocalY(float radians);
		void RotateLocalZ(float radians);
		void RotateWorldX(float radians);
		void RotateWorldY(float radians);
		void RotateWorldZ(float radians);

		DirectX::BoundingFrustum BuildViewSpaceBoundingFrustum() const;

		const DirectX::XMVECTOR& GetPosition() const;
		const DirectX::XMMATRIX& GetViewMatrix() const;
		const DirectX::XMMATRIX& GetProjectionMatrix() const;
		DirectX::XMVECTOR GetLocalRight() const;
		DirectX::XMVECTOR GetLocalUp() const;
		DirectX::XMVECTOR GetLocalForward() const;
		const DirectX::XMVECTOR& GetRotationQuaternion() const;

		Ray CreateRay() const;

		float GetNearZ() const;
		void SetNearZ(float value);

		float GetFarZ() const;
		void SetFarZ(float value);

		void SetPosition(float x, float y, float z);
		void XM_CALLCONV SetPosition(DirectX::FXMVECTOR position);
		void XM_CALLCONV SetRotationQuaternion(DirectX::FXMVECTOR rotationQuaternion);
		void SetAspectRatio(float aspectRatio);

		bool IsDirty() const;

		std::wstring ToWString() const;

	private:
		void XM_CALLCONV InitializeProjectionMatrix(DirectX::FXMMATRIX orientationMatrix);

	private:
		DirectX::XMMATRIX m_viewMatrix;
		DirectX::XMVECTOR m_position;
		DirectX::XMVECTOR m_rotationQuaternion;
		DirectX::XMMATRIX m_rotationMatrix;
		bool m_dirty;

		DirectX::XMMATRIX m_projectionMatrix;
		float m_aspectRatio;
		float m_fovAngleY;
		float m_nearZ;
		float m_farZ;
	};
}

#pragma once

#include <DirectXMath.h>
#include <DirectXCollision.h>

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
		void RotateLocalX(float radiansX);
		void RotateWorldY(float radiansY);

		DirectX::BoundingFrustum BuildViewSpaceBoundingFrustum() const;
		DirectX::BoundingFrustum BuildWorldSpaceBoundingFrustum() const;

		const DirectX::XMVECTOR& GetPosition() const;
		const DirectX::XMMATRIX& GetViewMatrix() const;
		const DirectX::XMMATRIX& GetProjectionMatrix() const;

		void SetPosition(float x, float y, float z);
		void SetAspectRatio(float aspectRatio);

		bool IsDirty() const;

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
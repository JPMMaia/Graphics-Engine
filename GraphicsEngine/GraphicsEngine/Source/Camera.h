#pragma once

#include <DirectXMath.h>
#include <DirectXCollision.h>

namespace GraphicsEngine
{
	class Camera
	{
	public:
		Camera();
		Camera(float aspectRatio, float fovAngleY, float nearZ, float farZ, const DirectX::XMFLOAT4X4& orientationMatrix);

		void Update();

		void MoveForward(float scalar);
		void MoveLeft(float scalar);
		void RotateLocalX(float angleX);
		void RotateWorldY(float angleY);

		DirectX::BoundingFrustum BuildViewSpaceBoundingFrustum() const;
		DirectX::BoundingFrustum BuildWorldSpaceBoundingFrustum() const;

		const DirectX::XMFLOAT3& GetPosition() const;
		const DirectX::XMFLOAT4X4& GetViewMatrix() const;
		const DirectX::XMFLOAT4X4& GetProjectionMatrix() const;

		void SetPosition(float x, float y, float z);

		bool IsDirty() const;

	private:
		void InitializeProjectionMatrix(const DirectX::XMFLOAT4X4& orientationMatrix);

	private:
		DirectX::XMFLOAT4X4 m_viewMatrix;
		DirectX::XMFLOAT3 m_position;
		DirectX::XMFLOAT3 m_rotation;
		DirectX::XMFLOAT3 m_left;
		DirectX::XMFLOAT3 m_up;
		DirectX::XMFLOAT3 m_forward;
		bool m_dirty;

		DirectX::XMFLOAT4X4 m_projectionMatrix;
		float m_aspectRatio;
		float m_fovAngleY;
		float m_nearZ;
		float m_farZ;
	};
}
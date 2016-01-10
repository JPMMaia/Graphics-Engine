#pragma once

#include <DirectXMath.h>

namespace GraphicsEngine
{
	class Camera
	{
	public:
		Camera();

		void Update();

		void MoveForward(float scalar);
		void MoveLeft(float scalar);
		void RotateLocalX(float angleX);
		void RotateWorldY(float angleY);

		const DirectX::XMFLOAT3& GetPosition() const;
		const DirectX::XMFLOAT4X4& GetViewMatrix() const;

		void SetPosition(float x, float y, float z);

		bool IsDirty() const;

	private:
		DirectX::XMFLOAT4X4 m_viewMatrix;
		DirectX::XMFLOAT3 m_position;
		DirectX::XMFLOAT3 m_rotation;

		DirectX::XMFLOAT3 m_left;
		DirectX::XMFLOAT3 m_up;
		DirectX::XMFLOAT3 m_forward;

		bool m_dirty;
	};
}
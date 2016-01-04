#include "stdafx.h"
#include "Camera.h"

using namespace DirectX;
using namespace GraphicsEngine;

Camera::Camera() :
	m_dirty(true)
{
	m_position = {};
	m_rotation = {};

	m_right = { 1.0f, 0.0f, 0.0f };
	m_up = { 0.0f, 1.0f, 0.0f };
	m_forward = { 0.0f, 0.0f, 1.0f };
}

void Camera::Update()
{
	if (m_dirty)
	{
		// Normalize the forward vector:
		auto forward = XMLoadFloat3(&m_forward);
		forward = XMVector3Normalize(forward);
		XMStoreFloat3(&m_forward, forward);

		// Orthonormalize the up vector:
		auto right = XMLoadFloat3(&m_right);
		auto up = XMVector3Normalize(XMVector3Cross(forward, right));
		XMStoreFloat3(&m_up, up);

		// Orthonormalize the right vector:
		right = XMVector3Cross(up, forward);
		XMStoreFloat3(&m_right, right);

		auto position = XMLoadFloat3(&m_position);

		// First column:
		m_viewMatrix(0, 0) = m_right.x;
		m_viewMatrix(1, 0) = m_right.y;
		m_viewMatrix(2, 0) = m_right.z;
		m_viewMatrix(3, 0) = -XMVectorGetX(XMVector3Dot(right, position));;

		// Second column:
		m_viewMatrix(0, 1) = m_up.x;
		m_viewMatrix(1, 1) = m_up.y;
		m_viewMatrix(2, 1) = m_up.z;
		m_viewMatrix(3, 1) = -XMVectorGetX(XMVector3Dot(up, position));

		// Third column:
		m_viewMatrix(0, 2) = m_forward.x;
		m_viewMatrix(1, 2) = m_forward.y;
		m_viewMatrix(2, 2) = m_forward.z;
		m_viewMatrix(3, 2) = -XMVectorGetX(XMVector3Dot(forward, position));

		// Fourth column:
		m_viewMatrix(0, 3) = 0.0f;
		m_viewMatrix(1, 3) = 0.0f;
		m_viewMatrix(2, 3) = 0.0f;
		m_viewMatrix(3, 3) = 1.0f;

		m_dirty = false;
	}
}

void Camera::MoveForward(float scalar)
{
	m_position.x -= scalar * m_forward.x;
	m_position.y -= scalar * m_forward.y;
	m_position.z -= scalar * m_forward.z;
	m_dirty = true;
}
void Camera::MoveRight(float scalar)
{
	m_position.x += scalar * m_right.x;
	m_position.y += scalar * m_right.y;
	m_position.z += scalar * m_right.z;
	m_dirty = true;
}
void Camera::RotateLocalX(float radians)
{
	auto rotationMatrix = XMMatrixRotationAxis(XMLoadFloat3(&m_right), -radians);

	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotationMatrix));
	XMStoreFloat3(&m_forward, XMVector3TransformNormal(XMLoadFloat3(&m_forward), rotationMatrix));

	m_dirty = true;
}
void Camera::RotateWorldY(float radians)
{
	auto rotationMatrix = XMMatrixRotationY(-radians);

	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), rotationMatrix));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotationMatrix));
	XMStoreFloat3(&m_forward, XMVector3TransformNormal(XMLoadFloat3(&m_forward), rotationMatrix));

	m_dirty = true;
}

const XMFLOAT3& Camera::GetPosition() const
{
	return m_position;
}
const XMFLOAT4X4& Camera::GetViewMatrix() const
{
	return m_viewMatrix;
}

void Camera::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
	m_dirty = true;
}

bool Camera::IsDirty() const
{
	return m_dirty;
}
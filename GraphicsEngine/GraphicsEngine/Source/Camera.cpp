#include "stdafx.h"
#include "Camera.h"

using namespace DirectX;
using namespace GraphicsEngine;

Camera::Camera() :
	m_dirty(true)
{
	m_position = {};
	m_rotation = {};

	m_left = { 1.0f, 0.0f, 0.0f };
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
		auto left = XMLoadFloat3(&m_left);
		auto up = XMVector3Normalize(XMVector3Cross(forward, left));
		XMStoreFloat3(&m_up, up);

		// Orthonormalize the left vector:
		left = XMVector3Cross(up, forward);
		XMStoreFloat3(&m_left, left);

		// Load position:
		auto position = XMLoadFloat3(&m_position);

		// Build view matrix:
		XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookToRH(position, forward, up));

		m_dirty = false;
	}
}

void Camera::MoveForward(float scalar)
{
	m_position.x += scalar * m_forward.x;
	m_position.y += scalar * m_forward.y;
	m_position.z += scalar * m_forward.z;
	m_dirty = true;
}
void Camera::MoveLeft(float scalar)
{
	m_position.x += scalar * m_left.x;
	m_position.y += scalar * m_left.y;
	m_position.z += scalar * m_left.z;
	m_dirty = true;
}
void Camera::RotateLocalX(float radians)
{
	auto rotationMatrix = XMMatrixRotationAxis(XMLoadFloat3(&m_left), radians);

	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotationMatrix));
	XMStoreFloat3(&m_forward, XMVector3TransformNormal(XMLoadFloat3(&m_forward), rotationMatrix));

	m_dirty = true;
}
void Camera::RotateWorldY(float radians)
{
	auto rotationMatrix = XMMatrixRotationY(radians);

	XMStoreFloat3(&m_left, XMVector3TransformNormal(XMLoadFloat3(&m_left), rotationMatrix));
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
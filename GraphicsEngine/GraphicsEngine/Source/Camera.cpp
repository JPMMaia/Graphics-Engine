#include "stdafx.h"
#include "Camera.h"

using namespace DirectX;
using namespace GraphicsEngine;

Camera::Camera() :
	m_dirty(true),
	m_aspectRatio(2.0f),
	m_fovAngleY(70.0f * XM_PI / 180.0f),
	m_nearZ(0.01f),
	m_farZ(100.0f)
{
}

Camera::Camera(float aspectRatio, float fovAngleY, float nearZ, float farZ, const XMFLOAT4X4& orientationMatrix) :
	m_dirty(true),
	m_aspectRatio(aspectRatio),
	m_fovAngleY(fovAngleY),
	m_nearZ(nearZ),
	m_farZ(farZ)
{
	m_position = {};
	m_rotation = {};

	m_left = { 1.0f, 0.0f, 0.0f };
	m_up = { 0.0f, 1.0f, 0.0f };
	m_forward = { 0.0f, 0.0f, 1.0f };

	// Build a perspective matrix:
	auto perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		nearZ,
		farZ
		);

	// Create a projection matrix by multiplying the perspective matrix with the orientation matrix:
	XMStoreFloat4x4(
		&m_projectionMatrix,
		perspectiveMatrix * XMLoadFloat4x4(&orientationMatrix)
		);
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
const XMFLOAT4X4& Camera::GetProjectionMatrix() const
{
	return m_projectionMatrix;
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
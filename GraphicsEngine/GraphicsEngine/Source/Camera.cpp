#include "stdafx.h"
#include "Camera.h"

using namespace DirectX;
using namespace GraphicsEngine;

Camera::Camera() :
	m_position(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
	m_rotationQuaternion(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
	m_rotationMatrix(),
	m_dirty(true),
	m_aspectRatio(2.0f),
	m_fovAngleY(70.0f * XM_PI / 180.0f),
	m_nearZ(0.01f),
	m_farZ(100.0f)
{
	// Initialize projection matrix, given orientation matrix equals to the identity matrix:
	InitializeProjectionMatrix(XMMatrixIdentity());
}

Camera::Camera(float aspectRatio, float fovAngleY, float nearZ, float farZ, const XMMATRIX& orientationMatrix) :
	m_position(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
	m_rotationQuaternion(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
	m_rotationMatrix(),
	m_dirty(true),
	m_aspectRatio(aspectRatio),
	m_fovAngleY(fovAngleY),
	m_nearZ(nearZ),
	m_farZ(farZ)
{
	InitializeProjectionMatrix(orientationMatrix);
}

void Camera::Update()
{
	if (m_dirty)
	{
		// Update the rotation matrix:
		m_rotationMatrix = XMMatrixRotationQuaternion(m_rotationQuaternion);
		
		// Build view matrix:
		auto up = m_rotationMatrix.r[1];
		auto forward = m_rotationMatrix.r[2];
		m_viewMatrix = XMMatrixLookToRH(m_position, forward, up);

		m_dirty = false;
	}
}

void Camera::Move(const XMVECTOR& axis, float scalar)
{
	// Calculate the translation:
	auto translation = XMVectorScale(axis, scalar);

	// Apply the translation:
	m_position = XMVectorAdd(m_position, translation);

	m_dirty = true;
}
void Camera::MoveLeft(float scalar)
{
	// Translate along the X-axis:
	const auto& left = m_rotationMatrix.r[0];
	Move(left, scalar);
}
void Camera::MoveForward(float scalar)
{
	// Translate along the Z-axis:
	const auto& forward = m_rotationMatrix.r[2];
	Move(forward, scalar);
}

void Camera::Rotate(const XMVECTOR& axis, float radians)
{
	// Calculate the rotation arround the axis:
	auto rotation = XMQuaternionRotationNormal(axis, radians);

	// Apply the rotation:
	m_rotationQuaternion = XMQuaternionMultiply(m_rotationQuaternion, rotation);

	m_dirty = true;
}
void Camera::RotateLocalX(float radians)
{	
	// Calculate the rotation arround the camera local X-axis:
	const auto& left = m_rotationMatrix.r[0];
	Rotate(left, radians);
}
void Camera::RotateWorldY(float radians)
{
	// Calculate the rotation arround the world Y-axis:
	static const auto WORLD_Y_AXIS = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	Rotate(WORLD_Y_AXIS, radians);
}

BoundingFrustum Camera::BuildViewSpaceBoundingFrustum() const
{
	BoundingFrustum boundingFrustum;
	BoundingFrustum::CreateFromMatrix(boundingFrustum, m_projectionMatrix);

	return boundingFrustum;
}
BoundingFrustum Camera::BuildWorldSpaceBoundingFrustum() const
{
	XMFLOAT3 position;
	XMStoreFloat3(&position, m_position);

	XMFLOAT4 orientation;
	XMStoreFloat4(&orientation, m_rotationQuaternion);

	// TODO
	auto rightSlope = 1.0f;
	auto leftSlope = 1.0f;
	auto topSlope = 1.0f;
	auto bottomSlope = 1.0f;
	
	return BoundingFrustum(position, orientation, rightSlope, leftSlope, topSlope, bottomSlope, m_nearZ, m_farZ);
}

const XMVECTOR& Camera::GetPosition() const
{
	return m_position;
}
const XMMATRIX& Camera::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}
const XMMATRIX& Camera::GetViewMatrix() const
{
	return m_viewMatrix;
}

void Camera::SetPosition(float x, float y, float z)
{
	m_position = XMVectorSet(x, y, z, 1.0f);
	m_dirty = true;
}

bool Camera::IsDirty() const
{
	return m_dirty;
}

void Camera::InitializeProjectionMatrix(const XMMATRIX& orientationMatrix)
{
	// Build a perspective matrix:
	auto perspectiveMatrix = XMMatrixPerspectiveFovRH(
		m_fovAngleY,
		m_aspectRatio,
		m_nearZ,
		m_farZ
		);

	// Create a projection matrix by multiplying the perspective matrix with the orientation matrix:
	m_projectionMatrix = perspectiveMatrix * orientationMatrix;
}
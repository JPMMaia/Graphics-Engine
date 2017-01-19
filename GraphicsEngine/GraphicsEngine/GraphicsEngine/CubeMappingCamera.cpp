#include "stdafx.h"
#include "CubeMappingCamera.h"

using namespace DirectX;
using namespace GraphicsEngine;

CubeMappingCamera::CubeMappingCamera(const DirectX::XMFLOAT3& position) :
	m_position(XMLoadFloat3(&position))
{
	BuildMatrices();
}

DirectX::XMVECTOR CubeMappingCamera::GetPosition() const
{
	return m_position;
}
const DirectX::XMMATRIX& CubeMappingCamera::GetViewMatrix(size_t index) const
{
	return m_viewMatrices[index];
}
const DirectX::XMMATRIX& CubeMappingCamera::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

float CubeMappingCamera::GetNearZ() const
{
	return 0.01f;
}
float CubeMappingCamera::GetFarZ() const
{
	return 1024.0f;
}

void CubeMappingCamera::BuildMatrices()
{
	std::array<XMFLOAT3, 6> eyesDirections =
	{
		XMFLOAT3(1.0f, 0.0f, 0.0f),
		XMFLOAT3(-1.0f, 0.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(0.0f, -1.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 1.0f),
		XMFLOAT3(0.0f, 0.0f, -1.0f),
	};
	std::array<XMFLOAT3, 6> upDirections =
	{
		XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, -1.0f),
		XMFLOAT3(0.0f, 0.0f, 1.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f),
	};

	for (size_t i = 0; i < 6; ++i)
		m_viewMatrices[i] = XMMatrixLookToLH(m_position, XMLoadFloat3(&eyesDirections[i]), XMLoadFloat3(&upDirections[i]));

	m_projectionMatrix = XMMatrixPerspectiveFovLH(XM_PI / 2.0f, 1.0f, GetNearZ(), GetFarZ());
}

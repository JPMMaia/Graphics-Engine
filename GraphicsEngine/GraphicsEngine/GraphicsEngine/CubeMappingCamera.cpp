#include "stdafx.h"
#include "CubeMappingCamera.h"

using namespace DirectX;
using namespace GraphicsEngine;

CubeMappingCamera::CubeMappingCamera(const DirectX::XMFLOAT3& position)
{
	BuildMatrices(position);
}

const DirectX::XMFLOAT4X4& CubeMappingCamera::GetViewMatrix(size_t index) const
{
	return m_viewMatrices[index];
}
const DirectX::XMFLOAT4X4& CubeMappingCamera::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

void CubeMappingCamera::BuildMatrices(const DirectX::XMFLOAT3& position)
{
	auto eyePosition = XMLoadFloat3(&position);
	std::array<XMVECTOR, 6> eyesDirections = 
	{
		XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f),
	};
	std::array<XMVECTOR, 6> upDirections =
	{
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, -1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
	};

	for(size_t i = 0; i < 6; ++i)
		XMStoreFloat4x4(&m_viewMatrices[i], XMMatrixLookToLH(eyePosition, eyesDirections[i], upDirections[i]));

	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixPerspectiveFovLH(XM_PI / 2.0f, 1.0f, 0.1f, 1000.0f));
}

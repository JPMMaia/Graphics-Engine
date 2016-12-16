#include "stdafx.h"
#include "Light.h"

using namespace DirectX;
using namespace GraphicsEngine;

XMFLOAT4X4 Light::GetViewMatrix(const DirectX::XMFLOAT3& focusPosition) const
{
	auto eyePosition = XMLoadFloat3(&m_lightData.Position);
	auto upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	
	XMFLOAT4X4 viewMatrix;
	XMStoreFloat4x4(&viewMatrix, XMMatrixLookAtLH(eyePosition, XMLoadFloat3(&focusPosition), upDirection));

	return viewMatrix;
}

DirectX::XMFLOAT4X4 Light::GetOrthographicMatrix(float viewWidth, float viewHeight, float nearZ, float farZ) const
{
	XMFLOAT4X4 orthographicMatrix;
	XMStoreFloat4x4(&orthographicMatrix, XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ));

	return orthographicMatrix;
}

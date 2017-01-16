#include "stdafx.h"
#include "Ray.h"

using namespace DirectX;
using namespace GraphicsEngine;

Ray::Ray(DirectX::FXMVECTOR origin, DirectX::FXMVECTOR direction) :
	m_origin(origin),
	m_direction(direction)
{
}
Ray::Ray(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction) :
	m_origin(XMLoadFloat3(&origin)),
	m_direction(XMVector3Normalize(XMLoadFloat3(&direction)))
{
}

DirectX::XMVECTOR Ray::CalculatePoint(float distance) const
{
	return m_origin + m_direction * distance;
}

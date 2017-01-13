#include "OctreeCollider.h"
#include "RenderItem.h"
#include "SubmeshGeometry.h"
#include "NormalRenderItem.h"

using namespace DirectX;
using namespace GraphicsEngine;

bool OctreeCollider::Intersects(const DirectX::BoundingBox& box) const
{
	BoundingBox worldSpaceBoundingBox;
	auto renderItemBounds = m_renderItem->GetSubmesh().Bounds;

	const auto& instanceData = m_renderItem->GetInstance(m_instanceID);
	renderItemBounds.Transform(worldSpaceBoundingBox, XMLoadFloat4x4(&instanceData.WorldMatrix));

	return worldSpaceBoundingBox.Intersects(box);
}
bool OctreeCollider::Intersects(const DirectX::BoundingFrustum& viewSpaceCameraFrustum, DirectX::FXMMATRIX inverseViewMatrix) const
{
	const auto& instanceData = m_renderItem->GetInstance(m_instanceID);

	// Get the world matrix of the instance and calculate its inverse:
	auto worldMatrix = XMLoadFloat4x4(&instanceData.WorldMatrix);
	auto worldMatrixDeterminant = XMMatrixDeterminant(worldMatrix);
	auto inverseWorldMatrix = XMMatrixInverse(&worldMatrixDeterminant, worldMatrix);

	// Calculate the matrix which transforms from view space to the instance's local space:
	auto viewToLocalMatrix = XMMatrixMultiply(inverseViewMatrix, inverseWorldMatrix);

	// Transform camera frustum from view space to local space:
	BoundingFrustum localSpaceCameraFrustum;
	viewSpaceCameraFrustum.Transform(localSpaceCameraFrustum, viewToLocalMatrix);

	// If the camera frustum intersects the instance bounds:
	if (localSpaceCameraFrustum.Contains(m_renderItem->GetSubmesh().Bounds) != ContainmentType::DISJOINT)
	{
		m_renderItem->InsertVisibleInstance(m_instanceID);
		return true;
	}

	return false;
}

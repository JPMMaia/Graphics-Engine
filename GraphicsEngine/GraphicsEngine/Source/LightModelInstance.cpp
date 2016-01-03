#include "stdafx.h"
#include "LightModelInstance.h"

using namespace DirectX;
using namespace GraphicsEngine;

LightModelInstance::LightModelInstance() :
	m_model(nullptr)
{
}
LightModelInstance::LightModelInstance(const LightModel* model) :
	m_model(model)
{
}

void LightModelInstance::Initialize(const LightModel* model)
{
	m_model = model;
}

void LightModelInstance::Draw(ID3D11DeviceContext1* d3dDeviceContext, LightEffect& lightEffect) const
{
	// Set the instance buffer:
	lightEffect.UpdatePerObjectConstantBuffer(d3dDeviceContext, m_instanceBuffer);

	// Draw the model:
	m_model->Draw(d3dDeviceContext, lightEffect);
}

void LightModelInstance::SetInstanceBuffer(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	auto modelMatrix = XMLoadFloat4x4(&world);
	auto viewMatrix = XMLoadFloat4x4(&view);
	auto projectionMatrix = XMLoadFloat4x4(&projection);

	// Store world matrix:
	XMStoreFloat4x4(&m_instanceBuffer.WorldMatrix, XMMatrixTranspose(modelMatrix));

	// Store world inverse transpose matrix:
	auto determinant = XMMatrixDeterminant(modelMatrix);
	XMStoreFloat4x4(&m_instanceBuffer.WorldInverseTransposeMatrix, XMMatrixInverse(&determinant, modelMatrix));

	// Store world view projection matrix:
	auto modelViewMatrix = XMMatrixMultiply(modelMatrix, viewMatrix);
	auto modelViewProjectionMatrix = XMMatrixMultiply(modelViewMatrix, projectionMatrix);
	XMStoreFloat4x4(&m_instanceBuffer.WorldViewProjectionMatrix, XMMatrixTranspose(modelViewProjectionMatrix));
}

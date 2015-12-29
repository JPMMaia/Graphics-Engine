#include "stdafx.h"
#include "Scene.h"
#include "VertexTypes.h";

using namespace DirectX;
using namespace GraphicsEngine;
using namespace std;

void Scene::Initialize(ID3D11Device* d3dDevice)
{
	static const vector<VertexPositionNormal> cubeVertices =
	{
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(-1.0f, -1.0f, -1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(-1.0f, -1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(-1.0f, 1.0f, -1.0f) },
		{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(-1.0f, 1.0f, 1.0f) },

		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, -1.0f, -1.0f) },
		{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(1.0f, -1.0f, 1.0f) },
		{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, -1.0f) },
		{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
	};
	static const vector<uint32_t> indices =
	{
		0, 2, 1,
		1, 2, 3,

		0, 1, 4,
		4, 1, 5,

		0, 4, 2,
		2, 4, 6,

		4, 5, 6,
		6, 5, 7,

		2, 6, 3,
		3, 6, 7,

		1, 3, 5,
		5, 3, 7,
	};

	m_effectManager.Initialize(d3dDevice);

	m_cubeMesh.Initialize(d3dDevice, cubeVertices, indices);
	
	m_frameBuffer.DirectionalLight =
	{
		XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f),
		XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
		XMFLOAT3(0.8f, 0.8f, 0.8f),
		1.0f,
		XMFLOAT3(1.0f, -1.0f, 0.0f)
	};

	m_cubeBuffer = {};

	// Set material:
	m_cubeBuffer.Material =
	{
		XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f),
		XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
		XMFLOAT3(0.8f, 0.8f, 0.8f),
		1.0f
	};

	m_cubeBuffer.EyePositionW = XMFLOAT3(0.0f, 0.7f, 1.5f);
}

void Scene::Reset()
{
	m_cubeMesh.Reset();
	m_effectManager.Reset();
}

void Scene::Render(ID3D11DeviceContext1* d3dDeviceContext)
{
	auto lightEffect = m_effectManager.GetLightEffect();

	lightEffect.Set(d3dDeviceContext);

	lightEffect.UpdatePerFrameConstantBuffer(d3dDeviceContext, m_frameBuffer);

	auto modelMatrix = XMLoadFloat4x4(&m_modelMatrix);
	auto viewMatrix = XMLoadFloat4x4(&m_viewMatrix);
	auto projectionMatrix = XMLoadFloat4x4(&m_projectionMatrix);

	// Store world matrix:
	XMStoreFloat4x4(&m_cubeBuffer.WorldMatrix, XMMatrixTranspose(modelMatrix));

	// Store world inverse transpose matrix:
	auto determinant = XMMatrixDeterminant(modelMatrix);
	XMStoreFloat4x4(&m_cubeBuffer.WorldInverseTransposeMatrix, XMMatrixInverse(&determinant, modelMatrix));

	// Store world view projection matrix:
	auto modelViewMatrix = XMMatrixMultiply(modelMatrix, viewMatrix);
	auto modelViewProjectionMatrix = XMMatrixMultiply(modelViewMatrix, projectionMatrix);
	XMStoreFloat4x4(&m_cubeBuffer.WorldViewProjectionMatrix, XMMatrixTranspose(modelViewProjectionMatrix));

	lightEffect.UpdatePerObjectConstantBuffer(d3dDeviceContext, m_cubeBuffer);
	m_cubeMesh.Draw(d3dDeviceContext);
}

void Scene::SetModelMatrix(const DirectX::XMFLOAT4X4& modelMatrix)
{
	m_modelMatrix = modelMatrix;
}

void Scene::SetViewMatrix(const DirectX::XMFLOAT4X4& viewMatrix)
{
	m_viewMatrix = viewMatrix;
}

void Scene::SetProjectionMatrix(const DirectX::XMFLOAT4X4& projectionMatrix)
{
	m_projectionMatrix = projectionMatrix;
}

#include "stdafx.h"
#include "Scene.h"
#include "VertexTypes.h"
#include "ModelBuilder.h"
#include "VirtualKey.h"

using namespace DirectX;
using namespace GraphicsEngine;
using namespace std;

void Scene::Initialize(ID3D11Device* d3dDevice)
{
	static const vector<VertexPositionNormalTexture> cubeVertices =
	{
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, -1.0f, -1.0f),  XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f), },
		{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
	};
	static const vector<uint32_t> cubeIndices =
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
	static const vector<Subset> subsets =
	{
		{ 0, cubeIndices.size() },
	};



	ModelBuilder builder(m_textureManager);
	m_cubeModel = builder.CreateFromX3D(d3dDevice, L"Resources/SimpleCube.x3d");

	m_cubeInstance.Initialize(&m_cubeModel);

	m_effectManager.Initialize(d3dDevice);
	m_frameBuffer.DirectionalLight =
	{
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),
		XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
		XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
		XMFLOAT3(1.0f, -1.0f, 0.0f)
	};
	m_frameBuffer.PointLight =
	{
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),
		XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
		XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
		XMFLOAT3(0.0f, 2.0f, 0.0f),
		5.0f,
		XMFLOAT3(1.0f, 1.0f, 1.0f)
	};
	m_frameBuffer.SpotLight =
	{
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f),
		XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
		XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
		XMFLOAT3(1.2f, 1.2f, 1.2f),
		5.0f,
		XMFLOAT3(0.0f, -1.0f, 0.0f),
		1.0f,
		XMFLOAT3(1.0f, 1.0f, 1.0f)
	};

	m_camera.SetPosition(0.0f, 1.4f, 3.0f);
	m_viewMatrix = m_camera.GetViewMatrix();

	XMStoreFloat4x4(&m_modelMatrix, XMMatrixIdentity());
}

void Scene::Reset()
{
	m_cubeModel.Reset();
	m_effectManager.Reset();
	m_textureManager.Reset();
}

void Scene::Render(ID3D11DeviceContext1* d3dDeviceContext)
{
	m_camera.Update();
	m_frameBuffer.EyePositionW = m_camera.GetPosition();
	m_viewMatrix = m_camera.GetViewMatrix();

	auto lightEffect = m_effectManager.GetLightEffect();

	lightEffect.Set(d3dDeviceContext);
	lightEffect.UpdatePerFrameConstantBuffer(d3dDeviceContext, m_frameBuffer);

	m_cubeInstance.SetInstanceBuffer(m_modelMatrix, m_viewMatrix, m_projectionMatrix);
	m_cubeInstance.Draw(d3dDeviceContext, lightEffect);
}

void Scene::SetProjectionMatrix(const DirectX::XMFLOAT4X4& projectionMatrix)
{
	m_projectionMatrix = projectionMatrix;
}

void Scene::HandleInput(const InputHandler& input)
{
	constexpr auto rotationScalar = 0.02f;
	if (input.IsKeyDown(VirtualKey::Right))
		m_camera.RotateWorldY(rotationScalar);

	if (input.IsKeyDown(VirtualKey::Up))
		m_camera.RotateLocalX(-rotationScalar);

	if (input.IsKeyDown(VirtualKey::Left))
		m_camera.RotateWorldY(-rotationScalar);

	if (input.IsKeyDown(VirtualKey::Down))
		m_camera.RotateLocalX(rotationScalar);

	constexpr auto translationScalar = 0.1f;
	if (input.IsKeyDown(VirtualKey::D))
		m_camera.MoveRight(translationScalar);

	if (input.IsKeyDown(VirtualKey::W))
		m_camera.MoveForward(translationScalar);

	if (input.IsKeyDown(VirtualKey::A))
		m_camera.MoveRight(-translationScalar);

	if (input.IsKeyDown(VirtualKey::S))
		m_camera.MoveForward(-translationScalar);
}

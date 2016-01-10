#include "stdafx.h"
#include "Scene.h"
#include "ModelBuilder.h"
#include "VirtualKey.h"

using namespace DirectX;
using namespace GraphicsEngine;
using namespace std;

void Scene::Initialize(ID3D11Device* d3dDevice)
{
	InitializeCubeModel(d3dDevice);
	InitializeFrameBuffer();
	InitializeTesselationBuffer();

	m_effectManager.Initialize(d3dDevice);
	m_camera.SetPosition(0.0f, 1.4f, 3.0f);
}

void Scene::Reset()
{
	m_cubeModel.Reset();
	m_effectManager.Reset();
	m_textureManager.Reset();
}

void Scene::Render(ID3D11DeviceContext1* d3dDeviceContext)
{
	// Update camera:
	UpdateCamera();

	auto lightEffect = m_effectManager.GetLightEffect();

	// Set light effect:
	lightEffect.Set(d3dDeviceContext);

	// Update constant buffers:
	lightEffect.UpdateFrameConstantBuffer(d3dDeviceContext, m_frameBuffer);
	lightEffect.UpdateCameraConstantBuffer(d3dDeviceContext, m_cameraBuffer);
	lightEffect.UpdateTesselationConstantBuffer(d3dDeviceContext, m_tesselationBuffer);

	// Draw cube instances:
	m_cubeModel.Draw(d3dDeviceContext, lightEffect, 125);
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

void Scene::InitializeCubeModel(ID3D11Device* d3dDevice)
{
	constexpr uint32_t size = 5;
	constexpr auto width = 20.0f;
	constexpr auto height = 20.0f;
	constexpr auto depth = 20.0f;
	constexpr auto x = -0.5f * width;
	constexpr auto y = -0.5f * height;
	constexpr auto z = -0.5f * depth;
	constexpr auto dx = width / (size - 1);
	constexpr auto dy = height / (size - 1);
	constexpr auto dz = depth / (size - 1);

	vector<LightEffect::InstanceData> instanceBuffer(size*size*size);
	for (auto i = 0; i < size; i++)
	{
		for (auto j = 0; j < size; j++)
		{
			for (auto k = 0; k < size; k++)
			{
				instanceBuffer[i*size*size + j*size + k].WorldMatrix = XMFLOAT4X4(
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					x + dx*i, y + dy*j, z + dz*k, 1.0f
					);
			}
		}
	}

	ModelBuilder builder(m_textureManager);
	//m_cubeModel = builder.CreateFromX3D(d3dDevice, L"Resources/SimpleCube.x3d", instanceBuffer);
	m_cubeModel = builder.CreateLightCube(d3dDevice, instanceBuffer);
}

void Scene::InitializeFrameBuffer()
{
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
}
void Scene::InitializeTesselationBuffer()
{
	m_tesselationBuffer.MaxTesselationDistance = 2.0f;
	m_tesselationBuffer.MinTesselationDistance = 10.0f;
	m_tesselationBuffer.MaxTesselationFactor = 8;
	m_tesselationBuffer.MinTesselationFactor = 1;
}

void Scene::UpdateCamera()
{
	// Update camera:
	m_camera.Update();

	// Update camera position in the constant buffer:
	m_cameraBuffer.EyePositionW = m_camera.GetPosition();

	// Build view projection matrix:
	auto& viewMatrix = m_camera.GetViewMatrix();
	auto viewProjectionMatrix = XMMatrixMultiply(XMLoadFloat4x4(&viewMatrix), XMLoadFloat4x4(&m_projectionMatrix));
	XMStoreFloat4x4(&m_cameraBuffer.ViewProjectionMatrix, XMMatrixTranspose(viewProjectionMatrix));
}
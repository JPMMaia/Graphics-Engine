#include "stdafx.h"
#include "Graphics.h"
#include "VertexTypes.h"

#include <array>
#include <DirectXColors.h>

using namespace DirectX;
using namespace GraphicsEngine;
using namespace std;

Graphics::Graphics(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight) :
	m_d3d(outputWindow, clientWidth, clientHeight)
{
	UpdateProjectionMatrix();

	// Reset the command list to prepare for initialization commands:
	auto commandList = m_d3d.GetCommandList();
	DX::ThrowIfFailed(commandList->Reset(m_d3d.GetCommandAllocator(), nullptr));

	// Initialize techniques:
	m_technique = Technique(m_d3d);

	// Initialize geometry:
	InitializeGeometry(m_d3d);

	// Execute the initialization commands:
	DX::ThrowIfFailed(commandList->Close());
	ID3D12CommandList* commandLists[] = { commandList };
	auto commandQueue = m_d3d.GetCommandQueue();
	commandQueue->ExecuteCommandLists(1, commandLists);

	// Wait until the initialization is complete:
	m_d3d.FlushCommandQueue();
}

void Graphics::OnResize(uint32_t clientWidth, uint32_t clientHeight)
{
	m_d3d.OnResize(clientWidth, clientHeight);

	UpdateProjectionMatrix();
}
void Graphics::Update(const Timer& timer)
{
	UpdateCamera();

	// Cycle through the circular frame resource array:
	m_currentFrameResourceIndex = (m_currentFrameResourceIndex + 1) % s_frameResourcesCount;
	m_currentFrameResource = m_frameResources[m_currentFrameResourceIndex].get();

	// Has the GPU finished processing the commands of the current frame resource.
	// If not, wait until the GPU has completed commands up to this fence point.
	auto fence = m_d3d.GetFence();
	if (m_currentFrameResource->Fence != 0 && fence->GetCompletedValue() < m_currentFrameResource->Fence)
	{
		auto eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
		DX::ThrowIfFailed(
			fence->SetEventOnCompletion(m_currentFrameResource->Fence, eventHandle)
			);
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	UpdateObjectConstantBuffer();
	UpdateMainPassConstantBuffer(timer);
}
void Graphics::Render(const Timer& timer)
{
	// Prepare scene to be drawn:
	m_d3d.BeginScene(m_technique.GetPipelineState());

	auto commandList = m_d3d.GetCommandList();

	// Apply technique:
	m_technique.Render(commandList);

	// Draw box:
	m_boxGeometry->Render(commandList);

	// Present the rendered scene to the screen:
	m_d3d.EndScene();
}

int Graphics::GetFrameResourcesCount()
{
	return s_frameResourcesCount;
}

void Graphics::InitializeGeometry(const D3DBase& d3dBase)
{
	array<VertexTypes::ColorVertexType, 8> vertices =
	{
		{
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) },
			{ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) },
			{ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) },
			{ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) },
			{ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) },
			{ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) },
			{ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) },
			{ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) }
		}
	};

	array<uint16_t, 36> indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	SubmeshGeometry submesh;
	submesh.IndexCount = static_cast<uint32_t>(indices.size());
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	m_boxGeometry = make_unique<MeshGeometry>();
	m_boxGeometry->Name = "Box Geometry";
	m_boxGeometry->Vertices = VertexBuffer(d3dBase, vertices.data(), static_cast<uint32_t>(vertices.size()), sizeof(VertexTypes::ColorVertexType));
	m_boxGeometry->Indices = IndexBuffer(d3dBase, indices.data(), static_cast<uint32_t>(indices.size()), sizeof(uint16_t), DXGI_FORMAT::DXGI_FORMAT_R16_UINT);
	m_boxGeometry->DrawArgs["Box"] = submesh;
}

void Graphics::UpdateProjectionMatrix()
{
	// Update the projection matrix with the new aspect ratio:
	XMStoreFloat4x4(
		&m_projectionMatrix,
		XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, m_d3d.GetAspectRatio(), 1.0f, 1000.0f)
		);
}

void Graphics::UpdateCamera()
{
	// Convert Spherical to Cartesian coordinates:
	m_eyePosition.x = m_radius*sinf(m_phi)*cosf(m_theta);
	m_eyePosition.z = m_radius*sinf(m_phi)*sinf(m_theta);
	m_eyePosition.y = m_radius*cosf(m_phi);

	// Build the view matrix:
	auto position = XMVectorSet(m_eyePosition.x, m_eyePosition.y, m_eyePosition.z, 1.0f);
	auto target = XMVectorZero();
	auto up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	auto view = XMMatrixLookAtLH(position, target, up);
	XMStoreFloat4x4(&m_viewMatrix, view);
}
void Graphics::UpdateObjectConstantBuffer()
{
	auto currentObjectConstantBuffer = m_currentFrameResource->ObjectConstants.get();
	for (auto& item : m_allRenderItems)
	{
		// Only update the cbuffer data if the constants have changed.
		// This needs to be tracked per frame resource.
		if (item->FramesDirtyCount > 0)
		{
			auto world = XMLoadFloat4x4(&item->WorldMatrix);

			ConstantBufferTypes::ObjectConstants objectConstants;
			XMStoreFloat4x4(&objectConstants.WorldMatrix, XMMatrixTranspose(world));

			currentObjectConstantBuffer->CopyData(item->ObjectCBIndex, objectConstants);

			// Next FrameResource need to be updated too:
			item->FramesDirtyCount--;
		}
	}
}
void Graphics::UpdateMainPassConstantBuffer(const Timer& timer)
{
	auto viewMatrix = XMLoadFloat4x4(&m_viewMatrix);
	auto viewMatrixDeterminant = XMMatrixDeterminant(viewMatrix);
	auto inverseViewMatrix = XMMatrixInverse(&viewMatrixDeterminant, viewMatrix);

	auto projectionMatrix = XMLoadFloat4x4(&m_projectionMatrix);
	auto projectionMatrixDeterminant = XMMatrixDeterminant(projectionMatrix);
	auto inverseProjectionMatrix = XMMatrixInverse(&projectionMatrixDeterminant, projectionMatrix);

	auto viewProjectionMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);
	auto viewProjectionMatrixDeterminant = XMMatrixDeterminant(viewProjectionMatrix);
	auto inverseViewProjectionMatrix = XMMatrixInverse(&viewProjectionMatrixDeterminant, viewProjectionMatrix);

	XMStoreFloat4x4(&m_passConstants.ViewMatrix, XMMatrixTranspose(viewMatrix));
	XMStoreFloat4x4(&m_passConstants.InverseViewMatrix, XMMatrixTranspose(inverseViewMatrix));
	XMStoreFloat4x4(&m_passConstants.ProjectionMatrix, XMMatrixTranspose(projectionMatrix));
	XMStoreFloat4x4(&m_passConstants.InverseProjectionMatrix, XMMatrixTranspose(inverseProjectionMatrix));
	XMStoreFloat4x4(&m_passConstants.ViewProjectionMatrix, XMMatrixTranspose(viewProjectionMatrix));
	XMStoreFloat4x4(&m_passConstants.InverseProjectionMatrix, XMMatrixTranspose(inverseViewProjectionMatrix));
	m_passConstants.EyePositionW = m_eyePosition;
	// TODO m_passConstants.RenderTargetSize = XMFLOAT2((float)m_d3d.Get, (float)mClientHeight);
	// TODO m_passConstants.InvRenderTargetSize = XMFLOAT2(1.0f / mClientWidth, 1.0f / mClientHeight);
	m_passConstants.NearZ = 1.0f;
	m_passConstants.FarZ = 1000.0f;
	m_passConstants.TotalTime = timer.GetTotalMilliseconds();
	// TODO m_passConstants.DeltaTime = timer.GetMillisecondsPerUpdate();

	auto currentPassCB = m_currentFrameResource->PassConstants.get();
	currentPassCB->CopyData(0, m_passConstants);
}

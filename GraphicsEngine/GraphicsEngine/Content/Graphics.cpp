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
}

void Graphics::OnResize(uint32_t clientWidth, uint32_t clientHeight)
{
	m_d3d.OnResize(clientWidth, clientHeight);

	// Update the projection matrix with the new aspect ratio:
	XMStoreFloat4x4(
		&m_projectionMatrix,
		XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, m_d3d.GetAspectRatio(), 1.0f, 1000.0f)
		);
}
void Graphics::Update(const Timer& timer)
{
	// Convert Spherical to Cartesian coordinates:
	auto x = m_radius * sinf(m_phi) * cosf(m_theta);
	auto y = m_radius * sinf(m_phi) * sinf(m_theta);
	auto z = m_radius * cosf(m_phi);

	// Build the view matrix:
	auto position = XMVectorSet(x, y, z, 1.0f);
	auto target = XMVectorZero();
	auto up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	auto view = XMMatrixLookAtLH(position, target, up);
	XMStoreFloat4x4(&m_viewMatrix, view);

	auto model = XMLoadFloat4x4(&m_modelMatrix);
	auto projection = XMLoadFloat4x4(&m_projectionMatrix);
	auto modelViewProjection = model * view * projection;

	// Update the constant buffer with the latest modelViewProjection matrix:
	PerObjectCBType perObjectCB;
	XMStoreFloat4x4(&perObjectCB.WorldViewProjectionMatrix, XMMatrixTranspose(modelViewProjection));
	m_technique.UpdatePerObjectCB(perObjectCB);
}
void Graphics::Render(const Timer& timer)
{
	auto commandAllocator = m_d3d.GetCommandAllocator();
	auto commandList = m_d3d.GetCommandList();

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	DX::ThrowIfFailed(commandAllocator->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	commandList->Reset(commandAllocator, m_technique.GetPipelineState());

	commandList->RSSetViewports(1, &m_d3d.GetScreenViewport());
	commandList->RSSetScissorRects(1, &m_d3d.GetScissorRect());

	// Indicate a state transition on the resource usage:
	auto transition1 = CD3DX12_RESOURCE_BARRIER::Transition(
		m_d3d.GetCurrentBackBuffer(), 
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT, 
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET
		);
	commandList->ResourceBarrier(1, &transition1);

	// Clear the back buffer and depth buffer:
	commandList->ClearRenderTargetView(m_d3d.GetCurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	commandList->ClearDepthStencilView(m_d3d.GetDepthStencilView(), D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to:

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
	m_boxGeometry->Indices = IndexBuffer(d3dBase, indices.data(), static_cast<uint32_t>(indices.size()), sizeof(uint16_t), DXGI_FORMAT::DXGI_FORMAT_R16G16_UINT);
	m_boxGeometry->DrawArgs["Box"] = submesh;
}

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
	// TODO
}
void Graphics::Render(const Timer& timer)
{
	// TODO
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

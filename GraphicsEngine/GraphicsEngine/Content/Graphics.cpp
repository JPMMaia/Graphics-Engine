#include "stdafx.h"
#include "Graphics.h"
#include "VertexTypes.h"

#include <array>
#include <DirectXColors.h>

using namespace DirectX;
using namespace GraphicsEngine;
using namespace Microsoft::WRL;
using namespace std;

Graphics::Graphics(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight) :
	m_d3d(outputWindow, clientWidth, clientHeight)
{
	UpdateProjectionMatrix();

	// Reset the command list to prepare for initialization commands:
	auto commandList = m_d3d.GetCommandList();
	DX::ThrowIfFailed(commandList->Reset(m_d3d.GetCommandAllocator(), nullptr));


	InitializeInputLayout();
	InitializeRootSignature(m_d3d);
	InitializeDescriptorHeaps(m_d3d);
	InitializeConstantBuffers(m_d3d);
	InitializePipelineState(m_d3d);
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
	m_d3d.BeginScene(m_pipelineState.Get());

	auto commandList = m_d3d.GetCommandList();
	
	{
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		commandList->SetGraphicsRootSignature(m_rootSignature.Get());
		commandList->SetGraphicsRootDescriptorTable(0, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
	}

	// Draw box:
	m_boxGeometry->Render(commandList);

	// Present the rendered scene to the screen:
	m_d3d.EndScene();
}

int Graphics::GetFrameResourcesCount()
{
	return s_frameResourcesCount;
}

void Graphics::InitializeInputLayout()
{
	m_inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}
void Graphics::InitializeRootSignature(const D3DBase& d3dBase)
{
	// Root parameter can be a table, a root descriptor or a root constant:
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];

	// Create a single descriptor table of CBVs:
	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

	// A root signature is an array of root parameters:
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDescription(1, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// Create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer:
	{
		ComPtr<ID3DBlob> serializedRootSignature;
		ComPtr<ID3DBlob> errorBlob = nullptr;
		auto hr = D3D12SerializeRootSignature(&rootSignatureDescription, D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSignature.GetAddressOf(), errorBlob.GetAddressOf());
		if (errorBlob != nullptr)
			::OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
		DX::ThrowIfFailed(hr);

		DX::ThrowIfFailed(
			d3dBase.GetDevice()->CreateRootSignature(
				0,
				serializedRootSignature->GetBufferPointer(),
				serializedRootSignature->GetBufferSize(),
				IID_PPV_ARGS(m_rootSignature.GetAddressOf())
				)
			);
	}
}
void Graphics::InitializeDescriptorHeaps(const D3DBase& d3dBase)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDescription;
	cbvHeapDescription.NumDescriptors = 1;
	cbvHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDescription.NodeMask = 0;

	DX::ThrowIfFailed(
		d3dBase.GetDevice()->CreateDescriptorHeap(&cbvHeapDescription, IID_PPV_ARGS(m_cbvHeap.GetAddressOf()))
		);
}
void Graphics::InitializeConstantBuffers(const D3DBase& d3dBase)
{
	auto d3dDevice = d3dBase.GetDevice();

	m_perObjectCB = std::make_unique<UploadBuffer<ConstantBufferTypes::ObjectConstants>>(d3dDevice, 1, true);

	auto cbByteSize = DX::CalculateConstantBufferByteSize(sizeof(ConstantBufferTypes::ObjectConstants));
	auto boxCbIndex = 0;
	auto cbGpuAddress = m_perObjectCB->GetResource()->GetGPUVirtualAddress();
	cbGpuAddress += boxCbIndex * cbByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDescription;
	cbvDescription.BufferLocation = cbGpuAddress;
	cbvDescription.SizeInBytes = cbByteSize;

	d3dDevice->CreateConstantBufferView(&cbvDescription, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());
}
void Graphics::InitializePipelineState(const D3DBase& d3dBase)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDescription = {};
	pipelineStateDescription.InputLayout.pInputElementDescs = &m_inputLayout[0];
	pipelineStateDescription.InputLayout.NumElements = static_cast<uint32_t>(m_inputLayout.size());
	pipelineStateDescription.pRootSignature = m_rootSignature.Get();
	pipelineStateDescription.VS = m_vertexShader.GetShaderBytecode();
	pipelineStateDescription.PS = m_pixelShader.GetShaderBytecode();
	pipelineStateDescription.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipelineStateDescription.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineStateDescription.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipelineStateDescription.SampleMask = UINT_MAX;
	pipelineStateDescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDescription.NumRenderTargets = 1;
	pipelineStateDescription.SampleDesc.Count = d3dBase.GetSampleCount();
	pipelineStateDescription.SampleDesc.Quality = d3dBase.GetSampleQuality();
	pipelineStateDescription.RTVFormats[0] = d3dBase.GetBackBufferFormat();
	pipelineStateDescription.DSVFormat = d3dBase.GetDepthStencilFormat();

	DX::ThrowIfFailed(
		d3dBase.GetDevice()->CreateGraphicsPipelineState(&pipelineStateDescription, IID_PPV_ARGS(m_pipelineState.GetAddressOf()))
		);
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
	m_passConstants.RenderTargetSize = XMFLOAT2(static_cast<float>(m_d3d.GetClientWidth()), static_cast<float>(m_d3d.GetClientHeight()));
	m_passConstants.InverseRenderTargetSize = XMFLOAT2(1.0f / static_cast<float>(m_d3d.GetClientWidth()), 1.0f / static_cast<float>(m_d3d.GetClientHeight()));
	m_passConstants.NearZ = 1.0f;
	m_passConstants.FarZ = 1000.0f;
	m_passConstants.TotalTime = timer.GetTotalMilliseconds();
	m_passConstants.DeltaTime = timer.GetDeltaMilliseconds();

	auto currentPassCB = m_currentFrameResource->PassConstants.get();
	currentPassCB->CopyData(0, m_passConstants);
}

#include "stdafx.h"
#include "Graphics.h"
#include "VertexTypes.h"

#include <array>
#include <DirectXColors.h>
#include "GeometryGenerator.h"
#include <D3Dcompiler.h>

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

	InitializeRootSignature();
	InitializeShadersAndInputLayout();
	InitializeGeometry();
	InitializeRenderItems();
	InitializeFrameResources();
	InitializeDescriptorHeaps();
	InitializeConstantBufferViews();
	InitializePipelineStateObjects();

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
	auto initialPipelineState = m_pipelineStateObjects["Opaque"];
	m_d3d.BeginScene(m_currentFrameResource->CommandAllocator.Get(), initialPipelineState.Get());

	auto commandList = m_d3d.GetCommandList();

	// Set descriptor heaps:
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	// Set root signature:
	commandList->SetGraphicsRootSignature(m_rootSignature.Get());

	// Set pass constant buffer:
	{
		auto passCbvIndex = static_cast<int>(m_allRenderItems.size() * s_frameResourcesCount + m_currentFrameResourceIndex);
		auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
		passCbvHandle.Offset(passCbvIndex, m_d3d.GetCbvSrvUavDescriptorSize());
		commandList->SetGraphicsRootDescriptorTable(1, passCbvHandle);
	}

	// Draw render items:
	DrawRenderItems(commandList, m_opaqueRenderItems);

	// Present the rendered scene to the screen:
	m_d3d.EndScene();

	// Advance the fence value to mark commands up to this fence point.
	auto currentFence = m_d3d.IncrementFence();
	m_currentFrameResource->Fence = currentFence;

	// Add an instruction to the command queue to set a new fence point. 
	// Because we are on the GPU timeline, the new fence point won't be 
	// set until the GPU finishes processing all the commands prior to this Signal().
	m_d3d.GetCommandQueue()->Signal(m_d3d.GetFence(), currentFence);
}

int Graphics::GetFrameResourcesCount()
{
	return s_frameResourcesCount;
}

void Graphics::InitializeShadersAndInputLayout()
{
	m_shaders["ColorVertexShader"] = Shader(L"ColorVertexShader.cso");
	m_shaders["ColorPixelShader"] = Shader(L"ColorPixelShader.cso");

	m_inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}
void Graphics::InitializeRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE cbvTable0;
	cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE cbvTable1;
	cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	// Create root CBVs:
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable0);
	slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable1);

	// A root signature is an array of root parameters:
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDescription(2, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// Create root signature:
	{
		ComPtr<ID3DBlob> serializedRootSignature;
		ComPtr<ID3DBlob> errorBlob = nullptr;
		auto hr = D3D12SerializeRootSignature(&rootSignatureDescription, D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSignature.GetAddressOf(), errorBlob.GetAddressOf());
		if (errorBlob != nullptr)
			::OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
		DX::ThrowIfFailed(hr);

		DX::ThrowIfFailed(
			m_d3d.GetDevice()->CreateRootSignature(
				0,
				serializedRootSignature->GetBufferPointer(),
				serializedRootSignature->GetBufferSize(),
				IID_PPV_ARGS(m_rootSignature.GetAddressOf())
				)
			);
	}
}
void Graphics::InitializeDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDescription;
	cbvHeapDescription.NumDescriptors = static_cast<uint32_t>((m_allRenderItems.size() + 1) * s_frameResourcesCount);
	cbvHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDescription.NodeMask = 0;

	DX::ThrowIfFailed(
		m_d3d.GetDevice()->CreateDescriptorHeap(&cbvHeapDescription, IID_PPV_ARGS(m_cbvHeap.GetAddressOf()))
		);
}
void Graphics::InitializeConstantBufferViews()
{
	auto d3dDevice = m_d3d.GetDevice();

	// Create objects constant buffer views:
	auto objectCBByteSize = DX::CalculateConstantBufferByteSize(sizeof(ConstantBufferTypes::ObjectConstants));
	for (auto frameIndex = 0; frameIndex < s_frameResourcesCount; ++frameIndex)
	{
		auto objectCB = m_frameResources[frameIndex]->ObjectConstants->GetResource();
		for (auto objectIndex = 0; objectIndex < m_allRenderItems.size(); ++objectIndex)
		{
			auto gpuAddress = objectCB->GetGPUVirtualAddress();
			gpuAddress += objectIndex * objectCBByteSize;

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDescription;
			cbvDescription.BufferLocation = gpuAddress;
			cbvDescription.SizeInBytes = objectCBByteSize;

			// Offset to the object cbv in the descriptor heap:
			auto heapIndex = static_cast<int>(frameIndex*m_allRenderItems.size() + objectIndex);
			auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cbvHeap->GetCPUDescriptorHandleForHeapStart());
			handle.Offset(heapIndex, m_d3d.GetCbvSrvUavDescriptorSize());

			d3dDevice->CreateConstantBufferView(&cbvDescription, handle);
		}
	}

	// Create pass constant buffers:
	auto passCBByteSize = DX::CalculateConstantBufferByteSize(sizeof(ConstantBufferTypes::PassConstants));
	for (auto frameIndex = 0; frameIndex < s_frameResourcesCount; ++frameIndex)
	{
		auto passCB = m_frameResources[frameIndex]->PassConstants->GetResource();

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDescription;
		cbvDescription.BufferLocation = passCB->GetGPUVirtualAddress();
		cbvDescription.SizeInBytes = passCBByteSize;

		auto heapIndex = static_cast<int>(m_allRenderItems.size() * s_frameResourcesCount + frameIndex);
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cbvHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(heapIndex, m_d3d.GetCbvSrvUavDescriptorSize());

		d3dDevice->CreateConstantBufferView(&cbvDescription, handle);
	}
}
void Graphics::InitializePipelineStateObjects()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePSODescription = {};
	opaquePSODescription.InputLayout.pInputElementDescs = &m_inputLayout[0];
	opaquePSODescription.InputLayout.NumElements = static_cast<uint32_t>(m_inputLayout.size());
	opaquePSODescription.pRootSignature = m_rootSignature.Get();
	opaquePSODescription.VS = m_shaders["ColorVertexShader"].GetShaderBytecode();
	opaquePSODescription.PS = m_shaders["ColorPixelShader"].GetShaderBytecode();
	opaquePSODescription.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePSODescription.RasterizerState.FillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
	opaquePSODescription.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePSODescription.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePSODescription.SampleMask = UINT_MAX;
	opaquePSODescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePSODescription.NumRenderTargets = 1;
	opaquePSODescription.SampleDesc.Count = m_d3d.GetSampleCount();
	opaquePSODescription.SampleDesc.Quality = m_d3d.GetSampleQuality();
	opaquePSODescription.RTVFormats[0] = m_d3d.GetBackBufferFormat();
	opaquePSODescription.DSVFormat = m_d3d.GetDepthStencilFormat();

	DX::ThrowIfFailed(
		m_d3d.GetDevice()->CreateGraphicsPipelineState(&opaquePSODescription, IID_PPV_ARGS(&m_pipelineStateObjects["Opaque"]))
		);

	auto opaqueWireframePSODescription = opaquePSODescription;
	opaqueWireframePSODescription.RasterizerState.FillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_WIREFRAME;

	DX::ThrowIfFailed(
		m_d3d.GetDevice()->CreateGraphicsPipelineState(&opaqueWireframePSODescription, IID_PPV_ARGS(&m_pipelineStateObjects["Opaque Wireframe"]))
		);

}
void Graphics::InitializeGeometry()
{
	auto box = GeometryGenerator::CreateBox(1.5f, 0.5f, 1.5f, 3);
	auto grid = GeometryGenerator::CreateGrid(20.0f, 30.0f, 60, 40);
	auto sphere = GeometryGenerator::CreateSphere(0.5f, 20, 20);
	auto cylinder = GeometryGenerator::CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

	//
	// We are concatenating all the geometry into one big vertex/index buffer.  So
	// define the regions in the buffer each submesh covers.
	//

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	uint32_t boxVertexOffset = 0;
	auto gridVertexOffset = static_cast<uint32_t>(box.Vertices.size());
	auto sphereVertexOffset = gridVertexOffset + static_cast<uint32_t>(grid.Vertices.size());
	auto cylinderVertexOffset = sphereVertexOffset + static_cast<uint32_t>(sphere.Vertices.size());

	// Cache the starting index for each object in the concatenated index buffer.
	uint32_t boxIndexOffset = 0;
	auto gridIndexOffset = static_cast<uint32_t>(box.Indices32.size());
	auto sphereIndexOffset = gridIndexOffset + static_cast<uint32_t>(grid.Indices32.size());
	auto cylinderIndexOffset = sphereIndexOffset + static_cast<uint32_t>(sphere.Indices32.size());

	// Define the SubmeshGeometry that cover different 
	// regions of the vertex/index buffers.

	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = static_cast<uint32_t>(box.Indices32.size());
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;

	SubmeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = static_cast<uint32_t>(grid.Indices32.size());
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;

	SubmeshGeometry sphereSubmesh;
	sphereSubmesh.IndexCount = static_cast<uint32_t>(sphere.Indices32.size());
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

	SubmeshGeometry cylinderSubmesh;
	cylinderSubmesh.IndexCount = static_cast<uint32_t>(cylinder.Indices32.size());
	cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	auto totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	vector<VertexTypes::ColorVertexType> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = box.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(Colors::DarkGreen);
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = grid.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(Colors::ForestGreen);
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = sphere.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(Colors::Crimson);
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = cylinder.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(Colors::SteelBlue);
	}

	vector<uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

	auto geometry = std::make_unique<MeshGeometry>();
	geometry->Name = "ShapeGeo";
	geometry->Vertices = VertexBuffer(m_d3d, vertices.data(), static_cast<uint32_t>(vertices.size()), sizeof(VertexTypes::ColorVertexType));
	geometry->Indices = IndexBuffer(m_d3d, indices.data(), static_cast<uint32_t>(indices.size()), sizeof(uint16_t), DXGI_FORMAT::DXGI_FORMAT_R16_UINT);
	geometry->DrawArgs["Box"] = boxSubmesh;
	geometry->DrawArgs["Grid"] = gridSubmesh;
	geometry->DrawArgs["Sphere"] = sphereSubmesh;
	geometry->DrawArgs["Cylinder"] = cylinderSubmesh;

	m_geometries[geometry->Name] = std::move(geometry);
}
void Graphics::InitializeRenderItems()
{
	auto boxRenderItem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&boxRenderItem->WorldMatrix, XMMatrixScaling(2.0f, 2.0f, 2.0f)*XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	boxRenderItem->ObjectCBIndex = 0;
	boxRenderItem->Mesh = m_geometries["ShapeGeo"].get();
	boxRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRenderItem->IndexCount = boxRenderItem->Mesh->DrawArgs["Box"].IndexCount;
	boxRenderItem->StartIndexLocation = boxRenderItem->Mesh->DrawArgs["Box"].StartIndexLocation;
	boxRenderItem->BaseVertexLocation = boxRenderItem->Mesh->DrawArgs["Box"].BaseVertexLocation;
	m_allRenderItems.push_back(std::move(boxRenderItem));

	auto gridRitem = std::make_unique<RenderItem>();
	gridRitem->WorldMatrix = MathHelper::Identity4x4();
	gridRitem->ObjectCBIndex = 1;
	gridRitem->Mesh = m_geometries["ShapeGeo"].get();
	gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Mesh->DrawArgs["Grid"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Mesh->DrawArgs["Grid"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Mesh->DrawArgs["Grid"].BaseVertexLocation;
	m_allRenderItems.push_back(std::move(gridRitem));

	UINT objCBIndex = 2;
	for (auto i = 0; i < 5; ++i)
	{
		auto leftCylRitem = std::make_unique<RenderItem>();
		auto rightCylRitem = std::make_unique<RenderItem>();
		auto leftSphereRitem = std::make_unique<RenderItem>();
		auto rightSphereRitem = std::make_unique<RenderItem>();

		XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i*5.0f);
		XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i*5.0f);

		XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i*5.0f);
		XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i*5.0f);

		XMStoreFloat4x4(&leftCylRitem->WorldMatrix, rightCylWorld);
		leftCylRitem->ObjectCBIndex = objCBIndex++;
		leftCylRitem->Mesh = m_geometries["ShapeGeo"].get();
		leftCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftCylRitem->IndexCount = leftCylRitem->Mesh->DrawArgs["Cylinder"].IndexCount;
		leftCylRitem->StartIndexLocation = leftCylRitem->Mesh->DrawArgs["Cylinder"].StartIndexLocation;
		leftCylRitem->BaseVertexLocation = leftCylRitem->Mesh->DrawArgs["Cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&rightCylRitem->WorldMatrix, leftCylWorld);
		rightCylRitem->ObjectCBIndex = objCBIndex++;
		rightCylRitem->Mesh = m_geometries["ShapeGeo"].get();
		rightCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightCylRitem->IndexCount = rightCylRitem->Mesh->DrawArgs["Cylinder"].IndexCount;
		rightCylRitem->StartIndexLocation = rightCylRitem->Mesh->DrawArgs["Cylinder"].StartIndexLocation;
		rightCylRitem->BaseVertexLocation = rightCylRitem->Mesh->DrawArgs["Cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&leftSphereRitem->WorldMatrix, leftSphereWorld);
		leftSphereRitem->ObjectCBIndex = objCBIndex++;
		leftSphereRitem->Mesh = m_geometries["ShapeGeo"].get();
		leftSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftSphereRitem->IndexCount = leftSphereRitem->Mesh->DrawArgs["Sphere"].IndexCount;
		leftSphereRitem->StartIndexLocation = leftSphereRitem->Mesh->DrawArgs["Sphere"].StartIndexLocation;
		leftSphereRitem->BaseVertexLocation = leftSphereRitem->Mesh->DrawArgs["Sphere"].BaseVertexLocation;

		XMStoreFloat4x4(&rightSphereRitem->WorldMatrix, rightSphereWorld);
		rightSphereRitem->ObjectCBIndex = objCBIndex++;
		rightSphereRitem->Mesh = m_geometries["ShapeGeo"].get();
		rightSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightSphereRitem->IndexCount = rightSphereRitem->Mesh->DrawArgs["Sphere"].IndexCount;
		rightSphereRitem->StartIndexLocation = rightSphereRitem->Mesh->DrawArgs["Sphere"].StartIndexLocation;
		rightSphereRitem->BaseVertexLocation = rightSphereRitem->Mesh->DrawArgs["Sphere"].BaseVertexLocation;

		m_allRenderItems.push_back(std::move(leftCylRitem));
		m_allRenderItems.push_back(std::move(rightCylRitem));
		m_allRenderItems.push_back(std::move(leftSphereRitem));
		m_allRenderItems.push_back(std::move(rightSphereRitem));
	}

	// All the render items are opaque.
	for (auto& e : m_allRenderItems)
		m_opaqueRenderItems.push_back(e.get());
}
void Graphics::InitializeFrameResources()
{
	for (auto i = 0; i < s_frameResourcesCount; ++i)
		m_frameResources.push_back(std::make_unique<FrameResource>(m_d3d.GetDevice(), 1, static_cast<uint32_t>(m_allRenderItems.size())));

	m_currentFrameResource = m_frameResources[m_currentFrameResourceIndex].get();
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
	m_passConstants.TotalTime = static_cast<float>(timer.GetTotalMilliseconds());
	m_passConstants.DeltaTime = static_cast<float>(timer.GetDeltaMilliseconds());

	auto currentPassCB = m_currentFrameResource->PassConstants.get();
	currentPassCB->CopyData(0, m_passConstants);
}

void Graphics::DrawRenderItems(ID3D12GraphicsCommandList* commandList, const std::vector<RenderItem*>& renderItems)
{
	// For each render item:
	for (size_t i = 0; i < renderItems.size(); ++i)
	{
		auto renderItem = renderItems[i];

		// Offset to the CBV in the descriptor heap for this object and for this frame resource:
		auto cbvIndex = static_cast<uint32_t>(m_currentFrameResourceIndex * m_allRenderItems.size() + renderItem->ObjectCBIndex);
		auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
		cbvHandle.Offset(cbvIndex, m_d3d.GetCbvSrvUavDescriptorSize());

		commandList->SetGraphicsRootDescriptorTable(0, cbvHandle);

		renderItem->Render(commandList);
	}
}

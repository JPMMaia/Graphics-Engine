#include "stdafx.h"
#include "Graphics.h"
#include "VertexTypes.h"

#include <array>
#include "GeometryGenerator.h"
#include <D3Dcompiler.h>
#include <DirectXColors.h>
#include "Material.h"

using namespace DirectX;
using namespace GraphicsEngine;
using namespace Microsoft::WRL;
using namespace std;

Graphics::Graphics(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight) :
	m_d3d(outputWindow, clientWidth, clientHeight),
	m_camera(m_d3d.GetAspectRatio(), 0.25f*DirectX::XM_PI, 1.0f, 1000.0f, XMMatrixIdentity())
{
	// Reset the command list to prepare for initialization commands:
	auto commandList = m_d3d.GetCommandList();
	DX::ThrowIfFailed(commandList->Reset(m_d3d.GetCommandAllocator(), nullptr));

	InitializeRootSignature();
	InitializeShadersAndInputLayout();
	InitializeGeometry();
	InitializeMaterials();
	InitializeRenderItems();
	InitializeFrameResources();
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
	m_camera.SetAspectRatio(m_d3d.GetAspectRatio());
}
void Graphics::Update(const Timer& timer)
{
	UpdateCamera();

	// Cycle through the circular frame resource array:
	m_currentFrameResourceIndex = (m_currentFrameResourceIndex + 1) % s_frameResourcesCount;
	m_currentFrameResource = m_frameResources[m_currentFrameResourceIndex].get();

	// Has the GPU finished processing the commands of the current frame resource?
	auto fence = m_d3d.GetFence();
	if (m_currentFrameResource->Fence != 0 && fence->GetCompletedValue() < m_currentFrameResource->Fence)
	{
		// If not, wait until the GPU has completed commands up to this fence point:
		auto eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
		DX::ThrowIfFailed(
			fence->SetEventOnCompletion(m_currentFrameResource->Fence, eventHandle)
			);
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	UpdateObjectsConstantBuffer();
	UpdateMaterialsConstantBuffer();
	UpdateMainPassConstantBuffer(timer);
}
void Graphics::Render(const Timer& timer)
{
	// Prepare scene to be drawn:
	auto initialPipelineState = m_pipelineStateObjects[m_wireframeEnabled ? "Opaque Wireframe" : "Opaque"].Get();
	m_d3d.BeginScene(m_currentFrameResource->CommandAllocator.Get(), initialPipelineState);

	auto commandList = m_d3d.GetCommandList();

	// Set root signature:
	commandList->SetGraphicsRootSignature(m_rootSignature.Get());

	// Set pass constant buffer:
	{
		auto passCB = m_currentFrameResource->PassConstantBuffer->GetResource();
		commandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());
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

Camera* Graphics::GetCamera()
{
	return &m_camera;
}
void Graphics::SetWireframeMode(bool enable)
{
	m_wireframeEnabled = enable;
}

int Graphics::GetFrameResourcesCount()
{
	return s_frameResourcesCount;
}

void Graphics::InitializeRootSignature()
{
	// Create root CBVs:
	std::array<CD3DX12_ROOT_PARAMETER, 3> slotRootParameter;
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);
	slotRootParameter[2].InitAsConstantBufferView(2);

	// A root signature is an array of root parameters:
	auto rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDescription(
		static_cast<uint32_t>(slotRootParameter.size()),	// Num root parameters
		slotRootParameter.data(),							// Root parameters
		0,													// Num static samplers
		nullptr,											// Static samplers
		rootSignatureFlags
		);

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
void Graphics::InitializeShadersAndInputLayout()
{
	wchar_t buffer[1024];
	GetModuleFileName(nullptr, buffer, 1024);
	OutputDebugStringW(buffer);

	m_shaders["DefaultVertexShader"] = Shader(L"Shaders/DefaultVertexShader.cso");
	m_shaders["DefaultPixelShader"] = Shader(L"Shaders/DefaultPixelShader.cso");

	m_inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
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
	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = static_cast<UINT>(box.Vertices.size());
	UINT sphereVertexOffset = gridVertexOffset + static_cast<UINT>(grid.Vertices.size());
	UINT cylinderVertexOffset = sphereVertexOffset + static_cast<UINT>(sphere.Vertices.size());

	// Cache the starting index for each object in the concatenated index buffer.
	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = static_cast<UINT>(box.Indices32.size());
	UINT sphereIndexOffset = gridIndexOffset + static_cast<UINT>(grid.Indices32.size());
	UINT cylinderIndexOffset = sphereIndexOffset + static_cast<UINT>(sphere.Indices32.size());

	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = static_cast<UINT>(box.Indices32.size());
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;

	SubmeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = static_cast<UINT>(grid.Indices32.size());
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;

	SubmeshGeometry sphereSubmesh;
	sphereSubmesh.IndexCount = static_cast<UINT>(sphere.Indices32.size());
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

	SubmeshGeometry cylinderSubmesh;
	cylinderSubmesh.IndexCount = static_cast<UINT>(cylinder.Indices32.size());
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

	std::vector<VertexTypes::DefaultVertexType> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].TextureCoordinates = box.Vertices[i].TextureCoordinates;
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
		vertices[k].TextureCoordinates = grid.Vertices[i].TextureCoordinates;
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = sphere.Vertices[i].Position;
		vertices[k].Normal = sphere.Vertices[i].Normal;
		vertices[k].TextureCoordinates = sphere.Vertices[i].TextureCoordinates;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = cylinder.Vertices[i].Position;
		vertices[k].Normal = cylinder.Vertices[i].Normal;
		vertices[k].TextureCoordinates = cylinder.Vertices[i].TextureCoordinates;
	}

	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "ShapeGeo";
	geo->Vertices = VertexBuffer(m_d3d, vertices.data(), static_cast<uint32_t>(vertices.size()), sizeof(VertexTypes::DefaultVertexType));
	geo->Indices = IndexBuffer(m_d3d, indices.data(), static_cast<uint32_t>(indices.size()), sizeof(std::uint16_t), DXGI_FORMAT::DXGI_FORMAT_R16_UINT);
	geo->DrawArgs["Box"] = boxSubmesh;
	geo->DrawArgs["Grid"] = gridSubmesh;
	geo->DrawArgs["Sphere"] = sphereSubmesh;
	geo->DrawArgs["Cylinder"] = cylinderSubmesh;

	m_geometries[geo->Name] = std::move(geo);

	// Load skull:
	auto skull = MeshGeometry::LoadFromFile(m_d3d, "Skull", L"Models/Skull.txt");
	m_geometries[skull->Name] = std::move(skull);
}
void Graphics::InitializeMaterials()
{
	auto bricks0 = std::make_unique<Material>();
	bricks0->Name = "Bricks0";
	bricks0->MaterialCBIndex = 0;
	bricks0->DiffuseSrvHeapIndex = 0;
	bricks0->DiffuseAlbedo = XMFLOAT4(Colors::ForestGreen);
	bricks0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	bricks0->Roughness = 0.1f;

	auto stone0 = std::make_unique<Material>();
	stone0->Name = "Stone0";
	stone0->MaterialCBIndex = 1;
	stone0->DiffuseSrvHeapIndex = 1;
	stone0->DiffuseAlbedo = XMFLOAT4(Colors::LightSteelBlue);
	stone0->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	stone0->Roughness = 0.3f;

	auto tile0 = std::make_unique<Material>();
	tile0->Name = "Tile0";
	tile0->MaterialCBIndex = 2;
	tile0->DiffuseSrvHeapIndex = 2;
	tile0->DiffuseAlbedo = XMFLOAT4(Colors::LightGray);
	tile0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	tile0->Roughness = 0.2f;

	auto skullMaterial = std::make_unique<Material>();
	skullMaterial->Name = "SkullMaterial";
	skullMaterial->MaterialCBIndex = 3;
	skullMaterial->DiffuseSrvHeapIndex = 3;
	skullMaterial->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	skullMaterial->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	skullMaterial->Roughness = 0.3f;

	m_materials["Bricks0"] = std::move(bricks0);
	m_materials["Stone0"] = std::move(stone0);
	m_materials["Tile0"] = std::move(tile0);
	m_materials["SkullMaterial"] = std::move(skullMaterial);
}
void Graphics::InitializeRenderItems()
{
	auto boxRenderItem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&boxRenderItem->WorldMatrix, XMMatrixScaling(2.0f, 2.0f, 2.0f)*XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	boxRenderItem->ObjectCBIndex = 0;
	boxRenderItem->Mesh = m_geometries["ShapeGeo"].get();
	boxRenderItem->Material = m_materials["Stone0"].get();
	boxRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRenderItem->IndexCount = boxRenderItem->Mesh->DrawArgs["Box"].IndexCount;
	boxRenderItem->StartIndexLocation = boxRenderItem->Mesh->DrawArgs["Box"].StartIndexLocation;
	boxRenderItem->BaseVertexLocation = boxRenderItem->Mesh->DrawArgs["Box"].BaseVertexLocation;
	m_allRenderItems.push_back(std::move(boxRenderItem));

	auto gridRenderItem = std::make_unique<RenderItem>();
	gridRenderItem->WorldMatrix = MathHelper::Identity4x4();
	gridRenderItem->ObjectCBIndex = 1;
	gridRenderItem->Mesh = m_geometries["ShapeGeo"].get();
	gridRenderItem->Material = m_materials["Tile0"].get();
	gridRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRenderItem->IndexCount = gridRenderItem->Mesh->DrawArgs["Grid"].IndexCount;
	gridRenderItem->StartIndexLocation = gridRenderItem->Mesh->DrawArgs["Grid"].StartIndexLocation;
	gridRenderItem->BaseVertexLocation = gridRenderItem->Mesh->DrawArgs["Grid"].BaseVertexLocation;
	m_allRenderItems.push_back(std::move(gridRenderItem));

	auto skullRenderItem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&skullRenderItem->WorldMatrix, XMMatrixScaling(0.5f, 0.5f, 0.5f)*XMMatrixTranslation(0.0f, 1.0f, 0.0f));
	skullRenderItem->ObjectCBIndex = 2;
	skullRenderItem->Material = m_materials["SkullMaterial"].get();
	skullRenderItem->Mesh = m_geometries["Skull"].get();
	skullRenderItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	skullRenderItem->IndexCount = skullRenderItem->Mesh->DrawArgs["Skull"].IndexCount;
	skullRenderItem->StartIndexLocation = skullRenderItem->Mesh->DrawArgs["Skull"].StartIndexLocation;
	skullRenderItem->BaseVertexLocation = skullRenderItem->Mesh->DrawArgs["Skull"].BaseVertexLocation;
	m_allRenderItems.push_back(std::move(skullRenderItem));

	UINT objCBIndex = 3;
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
		leftCylRitem->Material = m_materials["Bricks0"].get();
		leftCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftCylRitem->IndexCount = leftCylRitem->Mesh->DrawArgs["Cylinder"].IndexCount;
		leftCylRitem->StartIndexLocation = leftCylRitem->Mesh->DrawArgs["Cylinder"].StartIndexLocation;
		leftCylRitem->BaseVertexLocation = leftCylRitem->Mesh->DrawArgs["Cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&rightCylRitem->WorldMatrix, leftCylWorld);
		rightCylRitem->ObjectCBIndex = objCBIndex++;
		rightCylRitem->Mesh = m_geometries["ShapeGeo"].get();
		rightCylRitem->Material = m_materials["Bricks0"].get();
		rightCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightCylRitem->IndexCount = rightCylRitem->Mesh->DrawArgs["Cylinder"].IndexCount;
		rightCylRitem->StartIndexLocation = rightCylRitem->Mesh->DrawArgs["Cylinder"].StartIndexLocation;
		rightCylRitem->BaseVertexLocation = rightCylRitem->Mesh->DrawArgs["Cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&leftSphereRitem->WorldMatrix, leftSphereWorld);
		leftSphereRitem->ObjectCBIndex = objCBIndex++;
		leftSphereRitem->Mesh = m_geometries["ShapeGeo"].get();
		leftSphereRitem->Material = m_materials["Stone0"].get();
		leftSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftSphereRitem->IndexCount = leftSphereRitem->Mesh->DrawArgs["Sphere"].IndexCount;
		leftSphereRitem->StartIndexLocation = leftSphereRitem->Mesh->DrawArgs["Sphere"].StartIndexLocation;
		leftSphereRitem->BaseVertexLocation = leftSphereRitem->Mesh->DrawArgs["Sphere"].BaseVertexLocation;

		XMStoreFloat4x4(&rightSphereRitem->WorldMatrix, rightSphereWorld);
		rightSphereRitem->ObjectCBIndex = objCBIndex++;
		rightSphereRitem->Mesh = m_geometries["ShapeGeo"].get();
		rightSphereRitem->Material = m_materials["Stone0"].get();
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
		m_frameResources.push_back(std::make_unique<FrameResource>(m_d3d.GetDevice(), 1, static_cast<uint32_t>(m_allRenderItems.size()), static_cast<uint32_t>(m_materials.size())));

	m_currentFrameResource = m_frameResources[m_currentFrameResourceIndex].get();
}
void Graphics::InitializePipelineStateObjects()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePSODescription = {};
	opaquePSODescription.InputLayout.pInputElementDescs = &m_inputLayout[0];
	opaquePSODescription.InputLayout.NumElements = static_cast<uint32_t>(m_inputLayout.size());
	opaquePSODescription.pRootSignature = m_rootSignature.Get();
	opaquePSODescription.VS = m_shaders["DefaultVertexShader"].GetShaderBytecode();
	opaquePSODescription.PS = m_shaders["DefaultPixelShader"].GetShaderBytecode();
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
		m_d3d.GetDevice()->CreateGraphicsPipelineState(&opaquePSODescription, IID_PPV_ARGS(m_pipelineStateObjects["Opaque"].GetAddressOf()))
		);

	auto opaqueWireframePSODescription = opaquePSODescription;
	opaqueWireframePSODescription.RasterizerState.FillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_WIREFRAME;

	DX::ThrowIfFailed(
		m_d3d.GetDevice()->CreateGraphicsPipelineState(&opaqueWireframePSODescription, IID_PPV_ARGS(m_pipelineStateObjects["Opaque Wireframe"].GetAddressOf()))
		);

}

void Graphics::UpdateCamera()
{
	m_camera.Update();
}
void Graphics::UpdateObjectsConstantBuffer()
{
	auto currentObjectConstantBuffer = m_currentFrameResource->ObjectsConstantBuffer.get();
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
void Graphics::UpdateMaterialsConstantBuffer()
{
	auto currentMaterialCB = m_currentFrameResource->MaterialsConstantBuffer.get();
	for (auto& e : m_materials)
	{
		// Only update the cbuffer data if the constants have changed.  If the cbuffer
		// data changes, it needs to be updated for each FrameResource.
		auto material = e.second.get();
		if (material->FramesDirtyCount > 0)
		{
			auto materialTransform = XMLoadFloat4x4(&material->MatTransform);

			ConstantBufferTypes::MaterialConstants materialConstants;
			materialConstants.DiffuseAlbedo = material->DiffuseAlbedo;
			materialConstants.FresnelR0 = material->FresnelR0;
			materialConstants.Roughness = material->Roughness;
			XMStoreFloat4x4(&materialConstants.MaterialTransform, XMMatrixTranspose(materialTransform));

			currentMaterialCB->CopyData(material->MaterialCBIndex, materialConstants);

			// Next FrameResource need to be updated too.
			material->FramesDirtyCount--;
		}
	}
}
void Graphics::UpdateMainPassConstantBuffer(const Timer& timer)
{
	auto viewMatrix = m_camera.GetViewMatrix();
	auto viewMatrixDeterminant = XMMatrixDeterminant(viewMatrix);
	auto inverseViewMatrix = XMMatrixInverse(&viewMatrixDeterminant, viewMatrix);

	auto projectionMatrix = m_camera.GetProjectionMatrix();
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
	XMStoreFloat3(&m_passConstants.EyePositionW, m_camera.GetPosition());
	m_passConstants.RenderTargetSize = XMFLOAT2(static_cast<float>(m_d3d.GetClientWidth()), static_cast<float>(m_d3d.GetClientHeight()));
	m_passConstants.InverseRenderTargetSize = XMFLOAT2(1.0f / static_cast<float>(m_d3d.GetClientWidth()), 1.0f / static_cast<float>(m_d3d.GetClientHeight()));
	m_passConstants.NearZ = 1.0f;
	m_passConstants.FarZ = 1000.0f;
	m_passConstants.TotalTime = static_cast<float>(timer.GetTotalMilliseconds());
	m_passConstants.DeltaTime = static_cast<float>(timer.GetDeltaMilliseconds());
	m_passConstants.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
	m_passConstants.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
	m_passConstants.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
	m_passConstants.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	m_passConstants.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
	m_passConstants.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
	m_passConstants.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

	auto currentPassCB = m_currentFrameResource->PassConstantBuffer.get();
	currentPassCB->CopyData(0, m_passConstants);
}

void Graphics::DrawRenderItems(ID3D12GraphicsCommandList* commandList, const std::vector<RenderItem*>& renderItems) const
{
	auto objectCBByteSize = DX::CalculateConstantBufferByteSize(sizeof(ConstantBufferTypes::ObjectConstants));
	auto materialCBByteSize = DX::CalculateConstantBufferByteSize(sizeof(ConstantBufferTypes::MaterialConstants));

	auto objectCB = m_currentFrameResource->ObjectsConstantBuffer->GetResource();
	auto materialCB = m_currentFrameResource->MaterialsConstantBuffer->GetResource();

	// For each render item:
	for (size_t i = 0; i < renderItems.size(); ++i)
	{
		auto renderItem = renderItems[i];

		auto objectCBAddress = static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(objectCB->GetGPUVirtualAddress() + renderItem->ObjectCBIndex * objectCBByteSize);
		commandList->SetGraphicsRootConstantBufferView(0, objectCBAddress);

		auto materialCBAddress = static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(materialCB->GetGPUVirtualAddress() + renderItem->Material->MaterialCBIndex * materialCBByteSize);
		commandList->SetGraphicsRootConstantBufferView(1, materialCBAddress);

		renderItem->Render(commandList);
	}
}

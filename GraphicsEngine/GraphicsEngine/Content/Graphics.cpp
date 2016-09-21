#include "stdafx.h"
#include "Graphics.h"
#include "VertexTypes.h"
#include "Material.h"
#include "Samplers.h"

#include <array>
#include <D3Dcompiler.h>

using namespace DirectX;
using namespace GraphicsEngine;
using namespace Microsoft::WRL;
using namespace std;

Graphics::Graphics(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight) :
	m_d3d(outputWindow, clientWidth, clientHeight),
	m_camera(m_d3d.GetAspectRatio(), 0.25f*DirectX::XM_PI, 1.0f, 1000.0f, XMMatrixIdentity())
{
	m_camera.SetPosition(0.0f, 2.5f, -15.0f);

	// Reset the command list to prepare for initialization commands:
	auto commandList = m_d3d.GetCommandList();
	DX::ThrowIfFailed(commandList->Reset(m_d3d.GetCommandAllocator(), nullptr));

	InitializeRootSignature();
	m_pipelineStateManager = PipelineStateManager(m_d3d, m_rootSignature.Get(), m_postProcessRootSignature.Get());
	
	m_scene.AddTextures(&m_textureManager);
	m_textureManager.LoadAllTextures(m_d3d);

	m_descriptorHeap = DescriptorHeap(m_d3d, m_textureManager.GetTextureCount());
	m_textureManager.CreateShaderResourceViews(m_d3d, &m_descriptorHeap);

	m_scene.Initialize(this, m_d3d, m_textureManager);

	InitializeFrameResources();

	m_d3d.SetClearColor(m_passConstants.FogColor);

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
	auto initialPipelineState = m_pipelineStateManager.GetPipelineState(m_wireframeEnabled ? "Opaque Wireframe" : "Opaque");
	m_d3d.BeginScene(m_currentFrameResource->CommandAllocator.Get(), initialPipelineState);

	auto commandList = m_d3d.GetCommandList();

	// Set root signature:
	commandList->SetGraphicsRootSignature(m_rootSignature.Get());

	// Set pass constant buffer:
	{
		auto passCB = m_currentFrameResource->PassConstantBuffer->GetResource();
		commandList->SetGraphicsRootConstantBufferView(3, passCB->GetGPUVirtualAddress());
	}

	// Set texture descriptor heap:
	ID3D12DescriptorHeap* descriptorHeaps = { m_descriptorHeap.Get() };
	commandList->SetDescriptorHeaps(1, &descriptorHeaps);

	// Draw render items:
	{
		DrawRenderItems(commandList, m_renderItemLayers[static_cast<size_t>(RenderLayer::Opaque)]);

		m_pipelineStateManager.SetPipelineState(commandList, m_wireframeEnabled ? "Transparent Wireframe" : "Transparent");
		DrawRenderItems(commandList, m_renderItemLayers[static_cast<size_t>(RenderLayer::Transparent)]);

		m_pipelineStateManager.SetPipelineState(commandList, m_wireframeEnabled ? "Alpha Tested Wireframe" : "Alpha Tested");
		DrawRenderItems(commandList, m_renderItemLayers[static_cast<size_t>(RenderLayer::AlphaTested)]);

		m_pipelineStateManager.SetPipelineState(commandList, "Shadow");
		commandList->OMSetStencilRef(0);
		DrawRenderItems(commandList, m_renderItemLayers[static_cast<size_t>(RenderLayer::Shadow)]);
	}

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

void Graphics::AddRenderItem(unique_ptr<RenderItem>&& renderItem, initializer_list<RenderLayer> renderLayers)
{
	for (auto renderLayer : renderLayers)
	{
		m_renderItemLayers[static_cast<size_t>(renderLayer)].push_back(renderItem.get());
	}

	m_allRenderItems.push_back(std::move(renderItem));
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
	auto device = m_d3d.GetDevice();

	// Root signature:
	{
		// Describing textures tables:
		CD3DX12_DESCRIPTOR_RANGE texturesTable(
			D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
			1,
			0
			);

		// Specifying root parameters:
		std::array<CD3DX12_ROOT_PARAMETER, 4> slotRootParameter;
		slotRootParameter[0].InitAsDescriptorTable(1, &texturesTable, D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameter[1].InitAsConstantBufferView(0);
		slotRootParameter[2].InitAsConstantBufferView(1);
		slotRootParameter[3].InitAsConstantBufferView(2);

		auto staticSamplers = Samplers::GetStaticSamplers();

		// A root signature is an array of root parameters:
		auto rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDescription(
			static_cast<uint32_t>(slotRootParameter.size()),	// Num root parameters
			slotRootParameter.data(),							// Root parameters
			static_cast<uint32_t>(staticSamplers.size()),		// Num static samplers
			staticSamplers.data(),								// Static samplers
			rootSignatureFlags
			);

		// Create root signature:
		{
			ComPtr<ID3DBlob> serializedRootSignature;
			ComPtr<ID3DBlob> errorBlob = nullptr;
			auto hr = D3D12SerializeRootSignature(&rootSignatureDescription, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSignature.GetAddressOf(), errorBlob.GetAddressOf());
			if (errorBlob != nullptr)
				::OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
			DX::ThrowIfFailed(hr);

			DX::ThrowIfFailed(
				device->CreateRootSignature(
					0,
					serializedRootSignature->GetBufferPointer(),
					serializedRootSignature->GetBufferSize(),
					IID_PPV_ARGS(m_rootSignature.GetAddressOf())
					)
				);
		}
	}

	// Post process root signature:
	{
		CD3DX12_DESCRIPTOR_RANGE srvTable;
		srvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_DESCRIPTOR_RANGE uavTable;
		uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

		std::array<CD3DX12_ROOT_PARAMETER, 2> parameters;
		parameters[0].InitAsDescriptorTable(1, &srvTable);
		parameters[1].InitAsDescriptorTable(1, &uavTable);

		auto flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDescription(
			static_cast<UINT>(parameters.size()),
			parameters.data(),
			0,
			nullptr,
			flags
			);

		// Create root signature:
		{
			ComPtr<ID3DBlob> serializedRootSignature;
			ComPtr<ID3DBlob> errorBlob = nullptr;
			auto hr = D3D12SerializeRootSignature(&rootSignatureDescription, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSignature.GetAddressOf(), errorBlob.GetAddressOf());
			if (errorBlob != nullptr)
				::OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
			DX::ThrowIfFailed(hr);

			DX::ThrowIfFailed(
				device->CreateRootSignature(
					0,
					serializedRootSignature->GetBufferPointer(),
					serializedRootSignature->GetBufferSize(),
					IID_PPV_ARGS(m_postProcessRootSignature.GetAddressOf())
					)
				);
		}
	}
}

void Graphics::InitializeFrameResources()
{
	for (auto i = 0; i < s_frameResourcesCount; ++i)
		m_frameResources.push_back(std::make_unique<FrameResource>(m_d3d.GetDevice(), 1, static_cast<uint32_t>(m_allRenderItems.size()), static_cast<uint32_t>(m_scene.GetMaterials().size())));

	m_currentFrameResource = m_frameResources[m_currentFrameResourceIndex].get();
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
			ConstantBufferTypes::ObjectConstants objectConstants;

			auto worldMatrix = XMLoadFloat4x4(&item->WorldMatrix);
			XMStoreFloat4x4(&objectConstants.WorldMatrix, XMMatrixTranspose(worldMatrix));

			auto textureTransfrom = XMLoadFloat4x4(&item->TextureTransform);
			XMStoreFloat4x4(&objectConstants.TextureTransform, XMMatrixTranspose(textureTransfrom));

			currentObjectConstantBuffer->CopyData(item->ObjectCBIndex, objectConstants);

			// Next FrameResource need to be updated too:
			item->FramesDirtyCount--;
		}
	}
}
void Graphics::UpdateMaterialsConstantBuffer() const
{
	auto currentMaterialCB = m_currentFrameResource->MaterialsConstantBuffer.get();
	for (auto& e : m_scene.GetMaterials())
	{
		// Only update the cbuffer data if the constants have changed.  If the cbuffer
		// data changes, it needs to be updated for each FrameResource.
		auto material = e.second.get();
		if (material->FramesDirtyCount > 0)
		{
			auto materialTransform = XMLoadFloat4x4(&material->MaterialTransform);

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

		CD3DX12_GPU_DESCRIPTOR_HANDLE textureHandle(m_descriptorHeap.Get()->GetGPUDescriptorHandleForHeapStart());
		textureHandle.Offset(renderItem->Material->DiffuseSrvHeapIndex, m_d3d.GetCbvSrvUavDescriptorSize());
		commandList->SetGraphicsRootDescriptorTable(0, textureHandle);

		auto objectCBAddress = static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(objectCB->GetGPUVirtualAddress() + renderItem->ObjectCBIndex * objectCBByteSize);
		commandList->SetGraphicsRootConstantBufferView(1, objectCBAddress);

		auto materialCBAddress = static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(materialCB->GetGPUVirtualAddress() + renderItem->Material->MaterialCBIndex * materialCBByteSize);
		commandList->SetGraphicsRootConstantBufferView(2, materialCBAddress);

		renderItem->Render(commandList);
	}
}

#include "stdafx.h"
#include "DirectX12.h"
#include <cassert>

using namespace GraphicsEngine;
using namespace Microsoft::WRL;

DirectX12::DirectX12()
{
	// Create the ID3D12Device using the D3D12CreateDevice function:
	CreateDevice();

	// Create an ID3D12Fence object and query descriptor sizes:
	CreateFence();
	QueryDescriptorSizes();

	// Check 4X MSAA quality level support:
	Check4xMsaaQualityLevelSupport();

	// Create the command queue, command list allocator and main command list:
	CreateCommandQueueAndCommandList();

	// Describe and create the swap chain:
	CreateSwapChain();

	// Create the descriptor heaps the application requires:
	CreateDescriptorHeaps();

	// Resize the back buffer and create a render target view to the back buffer:
	CreateRenderTargetView();

	// Create the depth/stencil buffer and its associated depth/stencil view:
	CreateDepthStencilBufferAndView();

	// Set the viewport and scissor rectangles:
	SetViewportAndScissorRectangles();
}
DirectX12::~DirectX12()
{
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectX12::GetCurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
		m_currentBackBuffer,
		m_rtvDescriptorSize
		);
}
D3D12_CPU_DESCRIPTOR_HANDLE DirectX12::GetDepthStencilView() const
{
	return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void DirectX12::CreateDevice()
{
#if defined(_DEBUG)
	// Enable the D3D12 debug layer:
{
	ComPtr<ID3D12Debug> debugController;
	DX::ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())));
	debugController->EnableDebugLayer();
}
#endif

// Create DXGI factory:
DX::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory)));

// Try to create hardware device:
auto hardwareResult = D3D12CreateDevice(
	nullptr, // default device
	D3D_FEATURE_LEVEL_11_0,
	IID_PPV_ARGS(m_d3dDevice.GetAddressOf())
	);

// Fallback to WARP device if failed:
if (FAILED(hardwareResult))
{
	ComPtr<IDXGIAdapter> pWarpAdapter;
	DX::ThrowIfFailed(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(pWarpAdapter.GetAddressOf())));
	DX::ThrowIfFailed(
		D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(m_d3dDevice.GetAddressOf())
			)
		);
}
}
void DirectX12::CreateFence()
{
	DX::ThrowIfFailed(
		m_d3dDevice->CreateFence(
			0,
			D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(m_fence.GetAddressOf())
			)
		);
}
void DirectX12::QueryDescriptorSizes()
{
	m_rtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_dsvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_cbvSrvUavDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}
void DirectX12::Check4xMsaaQualityLevelSupport()
{
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS multisampleQualityLevels = {};
	multisampleQualityLevels.Format = m_backBufferFormat;
	multisampleQualityLevels.SampleCount = 4;
	multisampleQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	multisampleQualityLevels.NumQualityLevels = 0;

	DX::ThrowIfFailed(
		m_d3dDevice->CheckFeatureSupport(
			D3D12_FEATURE::D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&multisampleQualityLevels,
			sizeof(multisampleQualityLevels)
			)
		);

	m_multisampleNumQualityLevel = multisampleQualityLevels.NumQualityLevels;
	assert(m_multisampleNumQualityLevel > 0 && "Unexpected MSAA quality level.");
}
void DirectX12::CreateCommandQueueAndCommandList()
{
	// Create command queue:
	{
		D3D12_COMMAND_QUEUE_DESC queueDescription = {};
		queueDescription.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDescription.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		DX::ThrowIfFailed(
			m_d3dDevice->CreateCommandQueue(
				&queueDescription,
				IID_PPV_ARGS(m_commandQueue.GetAddressOf())
				)
			);
	}

	// Create command allocator:
	DX::ThrowIfFailed(
		m_d3dDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(m_commandAllocator.GetAddressOf())
			)
		);

	// Create command list:
	DX::ThrowIfFailed(
		m_d3dDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_commandAllocator.Get(),
			nullptr, // Initial PipelineStateObject
			IID_PPV_ARGS(m_commandList.GetAddressOf())
			)
		);

	// Start in a closed state:
	m_commandList->Close();
}
void DirectX12::CreateSwapChain()
{
	// Release the previous swap chain we will be recreating:
	m_dxgiSwapChain.Reset();

	// TODO
}
void DirectX12::CreateDescriptorHeaps()
{
	// Create RTV heap:
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDescription = {};
		rtvHeapDescription.NumDescriptors = s_swapChainBufferCount;
		rtvHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDescription.NodeMask = 0;

		DX::ThrowIfFailed(
			m_d3dDevice->CreateDescriptorHeap(
				&rtvHeapDescription, IID_PPV_ARGS(m_rtvHeap.GetAddressOf())
				)
			);
	}

	// Create DSV heap:
	{
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDescription = {};
		dsvHeapDescription.NumDescriptors = 1;
		dsvHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDescription.NodeMask = 0;

		DX::ThrowIfFailed(
			m_d3dDevice->CreateDescriptorHeap(
				&dsvHeapDescription, IID_PPV_ARGS(m_dsvHeap.GetAddressOf())
				)
			);
	}
}
void DirectX12::CreateRenderTargetView()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

	for (auto i = 0; i < s_swapChainBufferCount; ++i)
	{
		// Get the ith buffer in the swap chain:
		DX::ThrowIfFailed(
			m_dxgiSwapChain->GetBuffer(
				i,
				IID_PPV_ARGS(m_swapChainBuffer[i].GetAddressOf())
				)
			);

		// Create an RTV to it:
		m_d3dDevice->CreateRenderTargetView(m_swapChainBuffer[i].Get(), nullptr, rtvHeapHandle);

		// Next entry in heap:
		rtvHeapHandle.Offset(1, m_rtvDescriptorSize);
	}
}
void DirectX12::CreateDepthStencilBufferAndView()
{
	// Create the depth/stencil buffer:
	{
		D3D12_RESOURCE_DESC depthStencilDescription = {};
		depthStencilDescription.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDescription.Alignment = 0;
		depthStencilDescription.Width = m_clientWidth;
		depthStencilDescription.Height = m_clientHeight;
		depthStencilDescription.DepthOrArraySize = 1;
		depthStencilDescription.MipLevels = 1;
		depthStencilDescription.Format = m_depthStencilFormat;
		depthStencilDescription.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
		depthStencilDescription.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
		depthStencilDescription.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDescription.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optionClear = {};
		optionClear.Format = m_depthStencilFormat;
		optionClear.DepthStencil.Depth = 1.0f;
		optionClear.DepthStencil.Stencil = 0;

		CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

		DX::ThrowIfFailed(
			m_d3dDevice->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
				&depthStencilDescription,
				D3D12_RESOURCE_STATE_COMMON,
				&optionClear,
				IID_PPV_ARGS(m_depthStencilBuffer.GetAddressOf())
				)
			);
	}

	// Create a depth/stencil view:
	m_d3dDevice->CreateDepthStencilView(
		m_depthStencilBuffer.Get(),
		nullptr,
		GetDepthStencilView()
		);

	// Transisition the resource from its initial state to be used as a depth buffer:
	auto transition = CD3DX12_RESOURCE_BARRIER::Transition(
		m_depthStencilBuffer.Get(),
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE
		);
	m_commandList->ResourceBarrier(1, &transition);
}
void DirectX12::SetViewportAndScissorRectangles()
{
	// Set the viewport:
	{
		m_screenViewport = {};
		m_screenViewport.TopLeftX = 0.0f;
		m_screenViewport.TopLeftY = 0.0f;
		m_screenViewport.Width = static_cast<float>(m_clientWidth);
		m_screenViewport.Height = static_cast<float>(m_clientHeight);
		m_screenViewport.MinDepth = 0.0f;
		m_screenViewport.MaxDepth = 1.0f;

		m_commandList->RSSetViewports(1, &m_screenViewport);
	}

	// Set the scissor rectangles:
	{
		// TODO
	}
}

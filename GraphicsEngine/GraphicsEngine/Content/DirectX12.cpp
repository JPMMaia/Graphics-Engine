﻿#include "stdafx.h"
#include "DirectX12.h"

#include <cassert>
#include <string>
#include <vector>

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

	// TODO if debug log adapters
	LogAdapters();

	// Create the command queue, command list allocator and main command list:
	CreateCommandObjects();

	// Describe and create the swap chain:
	CreateSwapChain();

	// Create the descriptor heaps the application requires:
	CreateDescriptorHeaps();

	OnResize();
}
DirectX12::~DirectX12()
{
	if (m_d3dDevice != nullptr)
		FlushCommandQueue();
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

	m_4xMsaaQuality = multisampleQualityLevels.NumQualityLevels;
	assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");
}
void DirectX12::CreateCommandObjects()
{
	// Create command queue:
	{
		D3D12_COMMAND_QUEUE_DESC queueDescription = {};
		queueDescription.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDescription.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;

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

void DirectX12::OnResize()
{
	assert(m_d3dDevice);
	assert(m_dxgiSwapChain);
	assert(m_commandAllocator);

	// Flush before changing any resources:
	FlushCommandQueue();

	DX::ThrowIfFailed(
		m_commandList->Reset(m_commandAllocator.Get(), nullptr)
		);

	// Release the previous resources we will be recreating:
	for (auto i = 0; i < s_swapChainBufferCount; ++i)
		m_swapChainBuffer[i].Reset();
	m_depthStencilBuffer.Reset();

	// Resize the swap chain:
	m_dxgiSwapChain->ResizeBuffers(
		s_swapChainBufferCount,
		m_clientWidth,
		m_clientHeight,
		m_backBufferFormat,
		DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH // TODO not sure if this flag should be used in a UW app
		);

	m_currentBackBuffer = 0;

	CreateRenderTargetView();
	CreateDepthStencilBufferAndView();

	// Execute the resize commands:
	DX::ThrowIfFailed(m_commandList->Close());
	ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until resize is complete:
	FlushCommandQueue();

	SetViewportAndScissorRectangles();
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
	m_scissorRect = { 0, 0, m_clientWidth, m_clientHeight };
}

void DirectX12::FlushCommandQueue()
{
	// Advance the fence value to mark commands up to this fence point:
	m_currentFence++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	DX::ThrowIfFailed(
		m_commandQueue->Signal(m_fence.Get(), m_currentFence)
		);

	// Wait until the GPU has completed commands up to this fence point.
	if (m_fence->GetCompletedValue() < m_currentFence)
	{
		auto eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		DX::ThrowIfFailed(m_fence->SetEventOnCompletion(m_currentFence, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void DirectX12::GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter)
{
	ComPtr<IDXGIAdapter1> adapter;
	*ppAdapter = nullptr;

	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// Don't select the Basic Render Driver adapter.
			continue;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the
		// actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
		{
			break;
		}
	}

	*ppAdapter = adapter.Detach();
}

void DirectX12::LogAdapters() const
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while (m_dxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wstring text = L"***Adapter: ";
		text += desc.Description;
		text += L"\n";

		OutputDebugString(text.c_str());

		adapterList.push_back(adapter);

		++i;
	}

	for (size_t j = 0; j < adapterList.size(); ++j)
	{
		LogAdapterOutputs(adapterList[j]);
		adapterList[j]->Release();
	}
}
void DirectX12::LogAdapterOutputs(IDXGIAdapter* adapter) const
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"***Output: ";
		text += desc.DeviceName;
		text += L"\n";
		OutputDebugString(text.c_str());

		LogOutputDisplayModes(output, m_backBufferFormat);

		output->Release();

		++i;
	}
}
void DirectX12::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format) const
{
	UINT count = 0;
	UINT flags = 0;

	// Call with nullptr to get list count.
	output->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for (auto& x : modeList)
	{
		UINT n = x.RefreshRate.Numerator;
		UINT d = x.RefreshRate.Denominator;
		std::wstring text =
			L"Width = " + std::to_wstring(x.Width) + L" " +
			L"Height = " + std::to_wstring(x.Height) + L" " +
			L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
			L"\n";

		::OutputDebugString(text.c_str());
	}
}
#pragma once

#include <d3d12.h>
#include <dxgi1_5.h>
#include <wrl.h>
#include <cstdint>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace GraphicsEngine
{
	class DirectX12
	{
	public:
		explicit DirectX12();
		virtual ~DirectX12();
		//DirectX12(const DirectX12& rhs); = delete;
		//DirectX12& operator=(const DirectX12& rhs); = delete;
		

	public:
	
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

	protected:

		/// <summary>
		/// Create the ID3D12Device using the D3D12CreateDevice function.
		/// </summary>
		void CreateDevice();

		/// <summary>
		/// Create an ID3D12Fence object.
		/// </summary>
		void CreateFence();

		/// <summary>
		/// Query descriptor sizes.
		/// </summary>
		void QueryDescriptorSizes();

		/// <summary>
		/// Check 4X MSAA quality level support.
		/// </summary>
		void Check4xMsaaQualityLevelSupport();

		/// <sumary>
		/// Create the command queue, command list allocator and main command list.
		/// </sumary>
		void CreateCommandObjects();

		/// <sumary>
		/// Describe and create the swap chain.
		/// </sumary>
		void CreateSwapChain();

		/// <sumary>
		/// Create the descriptor heaps the application requires.
		/// </sumary>
		void CreateDescriptorHeaps();

		/// <sumary>
		/// Called when the window is resized.
		/// </sumary>
		void OnResize();

		/// <sumary>
		/// Resize the back buffer and create a render target view to the back buffer.
		/// </sumary>
		void CreateRenderTargetView();

		/// <sumary>
		/// Create the depth/stencil buffer and its associated depth/stencil view.
		/// </sumary>
		void CreateDepthStencilBufferAndView();

		/// <sumary>
		/// Set the viewport and scissor rectangles.
		/// </sumary>
		void SetViewportAndScissorRectangles();

		void FlushCommandQueue();

		/// <sumary>
		/// This method acquires the first available hardware adapter that supports Direct3D 12.
		/// If no such adapter can be found, *ppAdapter will be set to nullptr.
		/// </sumary>
		static void GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter);

		void LogAdapters() const;
		void LogAdapterOutputs(IDXGIAdapter* adapter) const;
		void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format) const;


	protected:
		bool m_4xMsaaState = false;
		uint32_t m_4xMsaaQuality = 0;

		Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_dxgiSwapChain;
		Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;
	
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		uint64_t m_currentFence = 0;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

		static constexpr uint64_t s_swapChainBufferCount = 2;
		int32_t m_currentBackBuffer = 0;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_swapChainBuffer[s_swapChainBufferCount];
		Microsoft::WRL::ComPtr<ID3D12Resource> m_depthStencilBuffer;
	
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;

		uint32_t m_rtvDescriptorSize = 0;
		uint32_t m_dsvDescriptorSize = 0;
		uint32_t m_cbvSrvUavDescriptorSize = 0;

		D3D12_VIEWPORT m_screenViewport;
		D3D12_RECT m_scissorRect;

		// Derived class should set these in derived constructor to customize starting values:
		DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		int32_t m_clientWidth = 800;
		int32_t m_clientHeight = 600;
	};
}

#include "stdafx.h"
#include "D3DBase.h"
#include "SettingsManager.h"

#include <dxgi1_2.h>

using namespace Common;
using namespace GraphicsEngine;
using namespace Microsoft::WRL;
using namespace std;

D3DBase::D3DBase(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight) :
	m_outputWindow(outputWindow),
	m_clientWidth(clientWidth),
	m_clientHeight(clientHeight)
{
	Initialize();
}

void D3DBase::OnResize(uint32_t clientWidth, uint32_t clientHeight)
{
	// TODO
}

void D3DBase::BeginScene() const
{
	// Clear the back buffer:
	m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), m_clearColor.data());

	// Clear the depth buffer:
	m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

}
void D3DBase::EndScene() const
{
	// Present as fast as possible:
	m_swapChain->Present(0, 0);
}

ID3D11Device* D3DBase::GetDevice() const
{
	return m_device.Get();
}
ID3D11DeviceContext* D3DBase::GetDeviceContext() const
{
	return m_deviceContext.Get();
}
float D3DBase::GetAspectRatio() const
{
	return static_cast<float>(m_clientWidth) / m_clientHeight;
}

void D3DBase::SetClearColor(const DirectX::XMFLOAT3 clearColor)
{
	m_clearColor[0] = clearColor.x;
	m_clearColor[1] = clearColor.y;
	m_clearColor[2] = clearColor.z;
	m_clearColor[3] = 1.0f;
}

void D3DBase::Initialize()
{
	InitializeDeviceResources();
	InitializeDepthStencilResources();

	// Bind the render target view and depth stencil buffer to the output render pipeline:
	m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

	InitializeRasterizerState();
	InitializeViewport();
}
void D3DBase::InitializeDeviceResources()
{
	// Create a DirectX graphics interface factory:
	ComPtr<IDXGIFactory2> factory;
	ThrowIfFailed(CreateDXGIFactory2(0, __uuidof(IDXGIFactory2), reinterpret_cast<void**>(factory.GetAddressOf())));

	// Create a settings manager object which will select the adapter:
	auto settingsManager = SettingsManager::Build(L"Settings.conf");

	// Use the factory to create an adapter for the primary graphics interface (video card):
	ComPtr<IDXGIAdapter1> adapter;
	ThrowIfFailed(factory->EnumAdapters1(settingsManager.GetAdapterIndex(), adapter.GetAddressOf()));

	// Initialize the swap chain description:
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1; // Single back buffer:
	swapChainDesc.BufferDesc.Width = m_clientWidth;
	swapChainDesc.BufferDesc.Height = m_clientHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = m_outputWindow;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = !m_fullscreen; // Set windowed mode
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // Discard the back buffer contents after presenting
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11.
	array<D3D_FEATURE_LEVEL, 2> featureLevels =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	// Create the swap chain, Direct3D device, and Direct3D device context.
	ThrowIfFailed(
		D3D11CreateDeviceAndSwapChain(
			adapter.Get(),
			D3D_DRIVER_TYPE_UNKNOWN,
			nullptr,
			0,
			featureLevels.data(),
			static_cast<UINT>(featureLevels.size()),
			D3D11_SDK_VERSION,
			&swapChainDesc,
			m_swapChain.GetAddressOf(),
			m_device.GetAddressOf(),
			nullptr,
			m_deviceContext.GetAddressOf()
		)
	);

	// Get the pointer to the back buffer.
	ComPtr<ID3D11Texture2D> backBuffer;
	ThrowIfFailed(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(backBuffer.GetAddressOf())));

	// Create the render target view with the back buffer pointer.
	ThrowIfFailed(m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf()));
}
void D3DBase::InitializeDepthStencilResources()
{
	// Create depth stencil buffer:
	{
		D3D11_TEXTURE2D_DESC depthStencilBufferDesc = {};
		depthStencilBufferDesc.Width = m_clientWidth;
		depthStencilBufferDesc.Height = m_clientHeight;
		depthStencilBufferDesc.MipLevels = 1;
		depthStencilBufferDesc.ArraySize = 1;
		depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilBufferDesc.SampleDesc.Count = 1;
		depthStencilBufferDesc.SampleDesc.Quality = 0;
		depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilBufferDesc.CPUAccessFlags = 0;
		depthStencilBufferDesc.MiscFlags = 0;

		// Create the texture for the depth buffer using the filled out description:
		ThrowIfFailed(m_device->CreateTexture2D(&depthStencilBufferDesc, nullptr, m_depthStencilBuffer.GetAddressOf()));
	}

	// Create the depth stencil state:
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

		// Depth state:
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		// Stencil state:
		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing:
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing:
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create the depth stencil state:
		ThrowIfFailed(m_device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.GetAddressOf()));

		// Set the depth stencil state:
		m_deviceContext->OMSetDepthStencilState(m_depthStencilState.Get(), 1);
	}

	// Create depth stencil view:
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		ThrowIfFailed(m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &depthStencilViewDesc, m_depthStencilView.GetAddressOf()));
	}
}
void D3DBase::InitializeRasterizerState()
{
	// Setup the raster description which will determine how and what polygons will be drawn:
	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	ThrowIfFailed(m_device->CreateRasterizerState(&rasterizerDesc, m_rasterizerState.GetAddressOf()));

	// Set the rasterizer state:
	m_deviceContext->RSSetState(m_rasterizerState.Get());
}
void D3DBase::InitializeViewport() const
{
	// Setup the viewport for rendering:
	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(m_clientWidth);
	viewport.Height = static_cast<float>(m_clientHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	m_deviceContext->RSSetViewports(1, &viewport);
}

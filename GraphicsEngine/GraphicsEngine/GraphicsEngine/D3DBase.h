#pragma once

#include <array>
#include <d3d11_2.h>
#include <dxgi1_3.h>
#include <wrl/client.h>

namespace GraphicsEngine
{
	/**
	 * This class was based on the RasterTek tutorial 3: Initializing DirectX 11.
	 * URL: http://www.rastertek.com/dx11s2tut03.html
	 */
	class D3DBase
	{
	public:
		explicit D3DBase(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight, bool fullscreen);

		void OnResize(uint32_t clientWidth, uint32_t clientHeight);

		void BeginScene() const;
		void EndScene() const;

		ID3D11Device2* GetDevice() const;
		ID3D11DeviceContext2* GetDeviceContext() const;
		float GetAspectRatio() const;
		uint32_t GetClientWidth() const;
		uint32_t GetClientHeight() const;

		void SetClearColor(const DirectX::XMFLOAT3 clearColor);

	private:
		void Initialize();
		void InitializeDeviceResources();
		void InitializeDepthStencilResources();
		void InitializeRasterizerState();
		void InitializeViewport();

	private:
		HWND m_outputWindow;
		uint32_t m_clientWidth;
		uint32_t m_clientHeight;
		bool m_fullscreen;
		
		Microsoft::WRL::ComPtr<IDXGISwapChain2> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D11Device2> m_device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext2> m_immediateContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;
		D3D11_VIEWPORT m_viewport;

		std::array<float, 4> m_clearColor;
	};
}

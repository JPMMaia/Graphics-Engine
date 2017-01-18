#pragma once

namespace GraphicsEngine
{
	class DynamicCubeMapRenderTexture
	{
	public:
		DynamicCubeMapRenderTexture() = default;
		DynamicCubeMapRenderTexture(ID3D11Device* d3dDevice, UINT width, UINT height, DXGI_FORMAT format);
		
		void SetRenderTarget(ID3D11DeviceContext* deviceContext, UINT index) const;
		void ClearRenderTarget(ID3D11DeviceContext* deviceContext, UINT index) const;
		ID3D11ShaderResourceView* GetShaderResourceView() const;

	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
		std::array<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, 6> m_renderTargetViews;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilTexture;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

		D3D11_VIEWPORT m_viewport;
	};
}

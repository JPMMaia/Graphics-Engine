#pragma once

namespace GraphicsEngine
{
	class RenderTexture
	{
	public:
		RenderTexture() = default;
		RenderTexture(ID3D11Device* d3dDevice, UINT width, UINT height, DXGI_FORMAT format);

		void SetRenderTargetView(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView) const;
		void ClearRenderTargetView(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, const DirectX::XMFLOAT4& color) const;
		ID3D11ShaderResourceView* GetShaderResourceView() const;

	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	};
}

#pragma once

namespace GraphicsEngine
{
	class ShadowTexture
	{
	public:
		ShadowTexture() = default;
		ShadowTexture(ID3D11Device* d3dDevice, size_t width, size_t height, DXGI_FORMAT format);

		void SetDepthStencilView(ID3D11DeviceContext* deviceContext) const;
		void ClearDepthStencilView(ID3D11DeviceContext* deviceContext) const;
		ID3D11ShaderResourceView* GetShaderResourceView() const;

	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	};
}

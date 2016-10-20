#pragma once

namespace GraphicsEngine
{
	class RasterizerState
	{
	public:
		RasterizerState();
		RasterizerState(ID3D11Device* d3dDevice, const D3D11_RASTERIZER_DESC& rasterizerDesc);

		void Initialize(ID3D11Device* d3dDevice, const D3D11_RASTERIZER_DESC& rasterizerDesc);
		void Reset();

		void Set(ID3D11DeviceContext* d3dDeviceContext) const;

		ID3D11RasterizerState* Get() const;
		ID3D11RasterizerState* const* GetAddressOf() const;

	private:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;
	};
}
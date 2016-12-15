#pragma once

namespace GraphicsEngine
{
	class DepthStencilState
	{
	public:
		DepthStencilState() = default;
		DepthStencilState(ID3D11Device* d3dDevice, const D3D11_DEPTH_STENCIL_DESC& depthStencilDesc);

		void Initialize(ID3D11Device* d3dDevice, const D3D11_DEPTH_STENCIL_DESC& depthStencilDesc);
		void Reset();

		void Set(ID3D11DeviceContext* d3dDeviceContext) const;

		ID3D11DepthStencilState* Get() const;
		ID3D11DepthStencilState* const* GetAddressOf() const;

	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
	};
}
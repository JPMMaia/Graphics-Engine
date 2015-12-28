#pragma once

namespace GraphicsEngine
{
	class RasterizerState
	{
	public:
		RasterizerState();

		void Reset();

		void Set(ID3D11DeviceContext1* d3dDeviceContext) const;

		ID3D11RasterizerState* Get() const;
		ID3D11RasterizerState** GetAddressOf();

	private:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;
	};
}
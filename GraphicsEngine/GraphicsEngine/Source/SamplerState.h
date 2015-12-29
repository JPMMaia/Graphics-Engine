#pragma once

namespace GraphicsEngine
{
	class SamplerState
	{
	public:
		SamplerState();
		SamplerState(ID3D11Device* d3dDevice, const D3D11_SAMPLER_DESC& samplerDesc);

		void Initialize(ID3D11Device* d3dDevice, const D3D11_SAMPLER_DESC& samplerDesc);
		void Reset();

		void VSSet(ID3D11DeviceContext1* d3dDeviceContext, uint32_t slot) const;
		void PSSet(ID3D11DeviceContext1* d3dDeviceContext, uint32_t slot) const;

		ID3D11SamplerState* Get() const;
		ID3D11SamplerState* const* GetAddressOf() const;

	private:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
	};
}
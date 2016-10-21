#pragma once

#include <d3d11_2.h>

namespace GraphicsEngine
{
	class SamplerState
	{
	public:
		SamplerState() = default;
		SamplerState(ID3D11Device* d3dDevice, const D3D11_SAMPLER_DESC& samplerDesc);

		void Initialize(ID3D11Device* d3dDevice, const D3D11_SAMPLER_DESC& samplerDesc);
		void Reset();

		ID3D11SamplerState* Get() const;
		ID3D11SamplerState* const* GetAddressOf() const;

	private:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
	};
}
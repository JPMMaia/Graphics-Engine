#pragma once

#include <d3d11_2.h>
#include <wrl/client.h>
#include <array>

namespace GraphicsEngine
{
	class BlendState
	{
	public:
		BlendState() = default;
		BlendState(ID3D11Device1* device, const D3D11_BLEND_DESC1& blendDesc, const std::array<float, 4>& blendFactor, UINT sampleMask);

		void Set(ID3D11DeviceContext* deviceContext) const;

		ID3D11BlendState1* Get() const;
		ID3D11BlendState1* const* GetAddressOf() const;

	private:
		Microsoft::WRL::ComPtr<ID3D11BlendState1> m_blendState;
		std::array<FLOAT, 4> m_blendFactor;
		UINT m_sampleMask;
	};
}

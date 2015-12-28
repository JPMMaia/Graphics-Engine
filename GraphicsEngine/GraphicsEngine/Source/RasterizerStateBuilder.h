#pragma once

#include "RasterizerState.h"

namespace GraphicsEngine
{
	class RasterizerStateBuilder
	{
	public:
		RasterizerStateBuilder();

		void SetCullMode(D3D11_CULL_MODE cullMode);
		void SetFillMode(D3D11_FILL_MODE fillMode);
		void SetFrontCounterClockwise(bool frontCounterClockwise);

		void Create(ID3D11Device* d3dDevice, RasterizerState& rasterizerState) const;
		void Create(ID3D11Device* d3dDevice, ID3D11RasterizerState** rasterizerState) const;

	private:
		D3D11_RASTERIZER_DESC m_rasterizerDesc;
	};
}
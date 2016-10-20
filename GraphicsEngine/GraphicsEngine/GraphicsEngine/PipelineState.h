#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "RasterizerState.h"

#include <d3d11_2.h>

namespace GraphicsEngine
{
	struct PipelineState
	{
	public:
		VertexShader* VertexShader;
		PixelShader* PixelShader;
		RasterizerState* RasterizerState;
		
		// ID3D11BlendState* BlendState;
		// ID3D11DepthStencilState* DepthStencilState;
		// UINT SampleMask;
		// UINT NumRenderTargets;
		// DXGI_SAMPLE_DESC SampleDesc;
		// std::array<DXGI_FORMAT, 8> RtvFormats;
		// DXGI_FORMAT DsvFormat;

	public:
		PipelineState() = default;
		
		void Set(ID3D11DeviceContext* deviceContext) const;
	};
}
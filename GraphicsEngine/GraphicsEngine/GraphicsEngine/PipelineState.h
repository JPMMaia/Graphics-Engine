#pragma once

#include "VertexShader.h"
#include "HullShader.h"
#include "DomainShader.h"
#include "PixelShader.h"
#include "RasterizerState.h"
#include "BlendState.h"
#include "DepthStencilState.h"

#include <d3d11_2.h>

namespace GraphicsEngine
{
	struct PipelineState
	{
	public:
		VertexShader* VertexShader = nullptr;
		HullShader* HullShader = nullptr;
		DomainShader* DomainShader = nullptr;
		PixelShader* PixelShader = nullptr;
		RasterizerState* RasterizerState = nullptr;
		BlendState* BlendState = nullptr;
		DepthStencilState* DepthStencilState = nullptr;

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

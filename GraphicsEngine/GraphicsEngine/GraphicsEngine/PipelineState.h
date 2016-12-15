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
		const VertexShader* VertexShader = nullptr;
		const HullShader* HullShader = nullptr;
		const DomainShader* DomainShader = nullptr;
		const PixelShader* PixelShader = nullptr;
		const RasterizerState* RasterizerState = nullptr;
		const BlendState* BlendState = nullptr;
		const DepthStencilState* DepthStencilState = nullptr;

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

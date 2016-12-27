#include "stdafx.h"
#include "PipelineState.h"

using namespace GraphicsEngine;

void PipelineState::Set(ID3D11DeviceContext* deviceContext) const
{
	// Set shaders:
	this->VertexShader->Set(deviceContext);
	this->HullShader->Set(deviceContext);
	this->DomainShader->Set(deviceContext);
	this->GeometryShader->Set(deviceContext);
	this->PixelShader->Set(deviceContext);

	// Set rasterizer state:
	this->RasterizerState->Set(deviceContext);

	// Set blend state:
	this->BlendState->Set(deviceContext);

	// Set depth stencil state:
	this->DepthStencilState->Set(deviceContext);
}

#include "stdafx.h"
#include "PipelineState.h"

using namespace GraphicsEngine;

void PipelineState::Set(ID3D11DeviceContext* deviceContext) const
{
	// Set shaders:
	this->VertexShader->Set(deviceContext);

	if (this->HullShader)
		this->HullShader->Set(deviceContext);
	else
		deviceContext->HSSetShader(nullptr, nullptr, 0);

	if (this->DomainShader)
		this->DomainShader->Set(deviceContext);
	else
		deviceContext->DSSetShader(nullptr, nullptr, 0);

	this->PixelShader->Set(deviceContext);

	// Set rasterizer state:
	this->RasterizerState->Set(deviceContext);

	// Set blend state:
	this->BlendState->Set(deviceContext);
}

#include "stdafx.h"
#include "PipelineState.h"

using namespace GraphicsEngine;

void PipelineState::Set(ID3D11DeviceContext* deviceContext) const
{
	// Set shaders:
	this->VertexShader->Set(deviceContext);
	this->PixelShader->Set(deviceContext);

	// Set rasterizer state:
	this->RasterizerState->Set(deviceContext);
}

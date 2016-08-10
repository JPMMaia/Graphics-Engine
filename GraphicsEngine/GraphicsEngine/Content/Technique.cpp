#include "stdafx.h"
#include "Technique.h"

using namespace GraphicsEngine;
using namespace std;

Technique::Technique(const wstring& vertexShaderFilename, const wstring& pixelShaderFilename) :
	m_vertexShader(vertexShaderFilename),
	m_pixelShader(pixelShaderFilename)
{
	InitializeRootSignature();
	InitializePipelineState();
	InitializeInputLayout();
}

void Technique::InitializeRootSignature()
{
}
void Technique::InitializePipelineState()
{
}
void Technique::InitializeInputLayout()
{
}

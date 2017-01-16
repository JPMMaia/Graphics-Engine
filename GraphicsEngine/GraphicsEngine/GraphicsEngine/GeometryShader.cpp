#include "stdafx.h"
#include "GeometryShader.h"

using namespace Common;
using namespace GraphicsEngine;
using namespace std;

const GeometryShader GeometryShader::s_null = GeometryShader();

GeometryShader::GeometryShader(ID3D11Device* d3dDevice, const std::wstring& filename)
{
	// Read data from domain shader file:
	vector<char> fileData;
	Helpers::ReadData(filename, fileData);
	Initialize(d3dDevice, &fileData[0], fileData.size());
}
GeometryShader::GeometryShader(ID3D11Device* d3dDevice, const std::wstring& filename, D3D11_SO_DECLARATION_ENTRY* streamOutputLayout, UINT streamOutputLayoutCount, UINT* bufferStrides, UINT bufferStridesCount, UINT rasterizedStream)
{
	// Read data from domain shader file:
	vector<char> fileData;
	Helpers::ReadData(filename, fileData);
	Initialize(d3dDevice, &fileData[0], fileData.size(), streamOutputLayout, streamOutputLayoutCount, bufferStrides, bufferStridesCount, rasterizedStream);
}

GeometryShader::GeometryShader(ID3D11Device* d3dDevice, const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
{
	auto blob = IShader::CompileFromFile(filename, defines, entrypoint, target);
	Initialize(d3dDevice, blob->GetBufferPointer(), blob->GetBufferSize());
}
GeometryShader::GeometryShader(ID3D11Device* d3dDevice, const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target, D3D11_SO_DECLARATION_ENTRY* streamOutputLayout, UINT streamOutputLayoutCount, UINT* bufferStrides, UINT bufferStridesCount, UINT rasterizedStream)
{
	auto blob = IShader::CompileFromFile(filename, defines, entrypoint, target);
	Initialize(d3dDevice, blob->GetBufferPointer(), blob->GetBufferSize(), streamOutputLayout, streamOutputLayoutCount, bufferStrides, bufferStridesCount, rasterizedStream);
}

void GeometryShader::Initialize(ID3D11Device* d3dDevice, const void* shaderByteCode, SIZE_T byteCodeLength)
{
	// Create geometry shader:
	ThrowIfFailed(
		d3dDevice->CreateGeometryShader(
			shaderByteCode,
			byteCodeLength,
			nullptr,
			m_geometryShader.GetAddressOf()
		)
	);
}

void GeometryShader::Initialize(ID3D11Device* d3dDevice, const void* shaderByteCode, SIZE_T byteCodeLength, D3D11_SO_DECLARATION_ENTRY* streamOutputLayout, UINT streamOutputLayoutCount, UINT* bufferStrides, UINT bufferStridesCount, UINT rasterizedStream)
{
	ThrowIfFailed(
		d3dDevice->CreateGeometryShaderWithStreamOutput(
			shaderByteCode,
			byteCodeLength,
			streamOutputLayout,
			streamOutputLayoutCount,
			bufferStrides,
			bufferStridesCount,
			rasterizedStream,
			nullptr,
			m_geometryShader.GetAddressOf()
		)
	);
}

void GeometryShader::Reset()
{
	m_geometryShader.Reset();
}

void GeometryShader::Set(ID3D11DeviceContext* d3dDeviceContext) const
{
	d3dDeviceContext->GSSetShader(m_geometryShader.Get(), nullptr, 0);
}
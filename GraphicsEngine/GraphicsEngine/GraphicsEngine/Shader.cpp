#include "stdafx.h"
#include "Shader.h"
#include "Common/EngineException.h"

#include <D3Dcompiler.h>

using namespace Common;
using namespace GraphicsEngine;
using namespace Microsoft::WRL;
using namespace std;

Shader Shader::CompileShader(const wstring& filename, const D3D_SHADER_MACRO* defines, const string& entrypoint, const string& target)
{
	UINT compileFlags;
#if defined(DEBUG) || defined(_DEBUG)  
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	compileFlags = 0;
#endif

	ComPtr<ID3DBlob> byteCode = nullptr;
	ComPtr<ID3DBlob> errors;
	auto hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
		OutputDebugStringA(static_cast<char*>(errors->GetBufferPointer()));

	ThrowIfFailed(hr);

	Shader shader;
	shader.m_byteCode = std::move(byteCode);

	return shader;
}

Shader::Shader(const wstring& filename)
{
	LoadBinary(filename);
}

void Shader::LoadBinary(const wstring& filename)
{
	ifstream filestream(filename, ios::binary);
	if (!filestream.good())
		ThrowEngineException(L"File not found!");

	filestream.seekg(0, ios_base::end);
	auto size = static_cast<size_t>(filestream.tellg());
	filestream.seekg(0, ios_base::beg);

	ThrowIfFailed(
		D3DCreateBlob(size, m_byteCode.GetAddressOf())
		);

	filestream.read(static_cast<char*>(m_byteCode->GetBufferPointer()), size);
	filestream.close();
}

D3D12_SHADER_BYTECODE Shader::GetShaderBytecode() const
{
	return{ m_byteCode->GetBufferPointer(), m_byteCode->GetBufferSize() };
}
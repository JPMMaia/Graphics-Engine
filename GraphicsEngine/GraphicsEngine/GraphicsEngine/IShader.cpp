#include "stdafx.h"
#include "IShader.h"
#include "Common/IncludeReplacer.h"

#pragma comment(lib, "d3dcompiler.lib")

#include <d3dcompiler.h>

using namespace Common;
using namespace GraphicsEngine;
using namespace Microsoft::WRL;
using namespace std;

ComPtr<ID3DBlob> IShader::CompileFromFile(const wstring& filename, const D3D_SHADER_MACRO* defines, const string& entrypoint, const string& target)
{
	const wstring* shaderFilename;

#if defined(DEBUG) || defined(_DEBUG)
	CreateDirectory(L"Generated", nullptr);
	auto generatedFilename = L"Generated/" + Helpers::GetFilename(filename) + L".hlsl";
	IncludeReplacer::ReplaceIncludes(filename, generatedFilename);
	shaderFilename = &generatedFilename;
#else
	shaderFilename = &filename;
#endif

	UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)  
	compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> byteCode = nullptr;
	ComPtr<ID3DBlob> errors;
	auto hr = D3DCompileFromFile(
		shaderFilename->c_str(),
		defines,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(),
		target.c_str(),
		compileFlags,
		0,
		byteCode.GetAddressOf(),
		errors.GetAddressOf()
	);

	if (errors != nullptr)
		OutputDebugStringA(static_cast<char*>(errors->GetBufferPointer()));

	ThrowIfFailed(hr);

	return byteCode;
}

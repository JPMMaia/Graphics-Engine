#include "stdafx.h"
#include "Texture.h"

#include "DDSTextureLoader.h"

using namespace GraphicsEngine;

Texture::Texture(const D3DBase& d3dBase, const std::string& name, const std::wstring& filename, int heapIndex) :
	Name(name),
	Filename(filename),
	HeapIndex(heapIndex)
{
	DX::ThrowIfFailed(
		DirectX::CreateDDSTextureFromFile12(
			d3dBase.GetDevice(),
			d3dBase.GetCommandList(),
			filename.c_str(),
			this->Resource,
			this->UploadHeap
			)
		);
}

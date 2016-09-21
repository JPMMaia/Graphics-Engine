#include "stdafx.h"
#include "Texture.h"

#include "DDSTextureLoader.h"

using namespace GraphicsEngine;

Texture::Texture(const std::string& name, const std::wstring& filename) :
	Name(name),
	Filename(filename)
{
}

void Texture::Load(const D3DBase& d3dBase)
{
	DX::ThrowIfFailed(
		DirectX::CreateDDSTextureFromFile12(
			d3dBase.GetDevice(),
			d3dBase.GetCommandList(),
			this->Filename.c_str(),
			this->Resource,
			this->UploadHeap
			)
		);
}

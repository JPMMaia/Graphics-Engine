#pragma once

#include "D3DBase.h"

namespace GraphicsEngine
{
	struct Texture
	{
	public:
		std::string Name;
		std::wstring Filename;
		Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
		int HeapIndex;

	public:
		Texture(const D3DBase& d3dBase, const std::string& name, const std::wstring& filename, int heapIndex);
	};
}

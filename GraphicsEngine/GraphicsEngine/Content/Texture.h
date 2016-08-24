#pragma once

#include <d3d12.h>
#include <string>
#include <wrl/client.h>

namespace GraphicsEngine
{
	struct Texture
	{
		std::string Name;
		std::wstring Filename;
		Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
	};
}

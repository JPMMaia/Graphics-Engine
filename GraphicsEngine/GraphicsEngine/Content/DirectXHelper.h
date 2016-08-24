#pragma once

#include "EngineException.h"

#include <wrl/client.h>
#include <comdef.h>

namespace GraphicsEngine
{
	namespace DX
	{
		inline void ThrowIfFailed(HRESULT hr);

		Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12Device* d3dDevice, ID3D12GraphicsCommandList* commandList, const void* initialData, uint64_t byteSize, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

		uint32_t CalculateConstantBufferByteSize(uint32_t byteSize);
		std::wstring AnsiToWString(const std::string& str);
	}
}

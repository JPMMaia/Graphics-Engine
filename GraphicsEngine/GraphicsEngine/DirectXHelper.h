#pragma once

#include <fstream>
#include <future>

namespace DX
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw GraphicsEngine::DirectXException(hr);
		}
	}
}
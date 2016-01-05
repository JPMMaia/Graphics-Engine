#pragma once

#include "DirectXException.h"

namespace GraphicsEngine
{
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
}
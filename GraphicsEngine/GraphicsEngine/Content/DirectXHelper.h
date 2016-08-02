#pragma once

#include <winerror.h>
#include <exception>

namespace GraphicsEngine
{
	namespace DX
	{
		inline void ThrowIfFailed(HRESULT hr)
		{
			if (FAILED(hr))
			{
				// Set a breakpoint on this line to catch Win32 API errors.
				throw std::exception();
			}
		}
	}
}

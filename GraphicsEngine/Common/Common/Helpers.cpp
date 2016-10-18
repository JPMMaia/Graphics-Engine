#include "Helpers.h"
#include "EngineException.h"

#include <comdef.h>

using namespace Common;

void Common::ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		_com_error error(hr);

		// Set a breakpoint on this line to catch Win32 API errors.
		ThrowEngineException(error.ErrorMessage());
	}
}

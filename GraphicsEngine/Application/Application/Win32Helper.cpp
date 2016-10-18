#include "Win32Helper.h"

#include <Windows.h>

void Win32Application::Win32Helper::DisplayError()
{
	// Retrieve the system error message for the last-error code:
	auto dw = GetLastError();

	LPVOID messageBuffer;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPTSTR>(&messageBuffer),
		0,
		nullptr
		);

	// Display the error message:
	MessageBox(nullptr, static_cast<LPCTSTR>(messageBuffer), TEXT("Error"), MB_OK);

	LocalFree(messageBuffer);
}

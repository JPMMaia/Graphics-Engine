#include "stdafx.h"
#include "DirectXException.h"

#include <array>

using namespace GraphicsEngine;
using namespace std;

DirectXException::DirectXException(HRESULT hr) : 
	exception()
{
	BuildErrorMessage(hr);
}

char const* DirectXException::what() const
{
	return m_errorMessage.data();
}

void DirectXException::BuildErrorMessage(HRESULT hr)
{
	array<wchar_t, 4096> buffer;
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
		buffer.data(),
		buffer.size(),
		nullptr
		);

	m_errorMessage = Helpers::WStringToString(buffer.data());
}

#include "Helpers.h"
#include "EngineException.h"

#include <comdef.h>

using namespace Common;

bool Helpers::FileExists(const std::wstring& filename)
{
	std::fstream fileStream(filename);

	return fileStream.good();
}
std::wstring Helpers::GetFileExtension(const std::wstring& filename)
{
	for (auto it = filename.end() - 1; it != filename.begin(); --it)
	{
		if (*it == '.')
			return std::wstring(it + 1, filename.end());
	}

	ThrowEngineException(L"Filename has no extension.");
}

std::wstring Helpers::StringToWString(const std::string& str)
{
	using convertType = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convertType, wchar_t> converter;

	return converter.from_bytes(str);
}

std::string Helpers::WStringToString(const std::wstring& wstr)
{
	using convertType = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convertType, wchar_t> converter;

	return converter.to_bytes(wstr);
}

void Helpers::ReadData(const std::wstring& filename, std::vector<char>& buffer)
{
	using namespace std;

	// Open file for reading in binary mode, and seek to the end of file immediately:
	ifstream file(filename, ios::in | ios::binary | ios::ate);
	if (!file.good())
		throw runtime_error("Couldn't open file " + Helpers::WStringToString(filename));

	// Get size of file and seek to the begin of file:
	auto size = file.tellg();
	file.seekg(0, ios::beg);

	// Read content of file:
	buffer.resize(static_cast<uint32_t>(size));
	file.read(buffer.data(), size);
	if (!file.good())
		throw runtime_error("Error while reading file " + Helpers::WStringToString(filename));
}

void Common::ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		_com_error error(hr);

		// Set a breakpoint on this line to catch Win32 API errors.
		ThrowEngineException(error.ErrorMessage());
	}
}

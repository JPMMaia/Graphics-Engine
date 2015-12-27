#pragma once

#include <fstream>
#include <codecvt>

namespace Helpers
{
	inline bool FileExists(const std::wstring& filename)
	{
		std::fstream fileStream(filename);

		return fileStream.good();
	}

	inline std::wstring StringToWString(const std::string& str)
	{
		using convertType = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convertType, wchar_t> converter;

		return converter.from_bytes(str);
	}

	inline std::string WStringToString(const std::wstring& wstr)
	{
		using convertType = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convertType, wchar_t> converter;

		return converter.to_bytes(wstr);
	}
}

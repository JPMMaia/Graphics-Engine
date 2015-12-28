#pragma once

#include <codecvt>
#include <future>
#include <fstream>

namespace GraphicsEngine
{
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

		inline void ReadData(const std::wstring& filename, std::vector<char>& buffer)
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

		template<typename FunctionType, typename... ArgumentsType>
		std::future<typename std::result_of<FunctionType(ArgumentsType...)>::type> RunAsync(FunctionType&& function, ArgumentsType&&... arguments)
		{
			return std::async(std::launch::async, std::forward<FunctionType>(function), std::forward<ArgumentsType>(arguments)...);
		}
	}

}
#pragma once

#include <codecvt>
#include <future>
#include <fstream>
#include <windows.h>
#include "EngineException.h"

#include <DirectXMath.h>

namespace Common
{
	namespace Helpers
	{
		bool FileExists(const std::wstring& filename);
		std::wstring GetFilename(const std::wstring& filename);
		std::wstring GetFileExtension(const std::wstring& filename);
		std::wstring GetFilePath(const std::wstring& filename);

		std::wstring StringToWString(const std::string& str);
		std::string WStringToString(const std::wstring& wstr);

		template<typename DataType>
		void ReadData(const std::wstring& filename, std::vector<DataType>& buffer)
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
			buffer.resize(static_cast<uint32_t>(size) / sizeof(DataType));
			file.read(reinterpret_cast<char*>(buffer.data()), size);
			if (!file.good())
				throw runtime_error("Error while reading file " + Helpers::WStringToString(filename));
		}
		void WriteData(const std::wstring& filename, const std::vector<char>& buffer);

		template<typename FunctionType, typename... ArgumentsType>
		std::future<typename std::result_of<FunctionType(ArgumentsType...)>::type> RunAsync(FunctionType&& function, ArgumentsType&&... arguments)
		{
			return std::async(std::launch::async, std::forward<FunctionType>(function), std::forward<ArgumentsType>(arguments)...);
		}

		inline std::wstring AnsiToWString(const std::string& str)
		{
			WCHAR buffer[512];
			MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
			return std::wstring(buffer);
		}
	}

	void ThrowIfFailed(HRESULT hr);
}

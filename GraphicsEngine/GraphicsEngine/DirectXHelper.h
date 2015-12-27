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

	inline bool ReadData(const std::wstring& filename, std::vector<char>& buffer)
	{
		using namespace std;

		// Open file for reading in binary mode, and seek to the end of file immediately:
		ifstream file(filename, ios::in | ios::binary | ios::ate);
		
		// Get size of file and seek to the begin of file:
		auto size = file.tellg();
		file.seekg(0, ios::beg);

		// Read content of file:
		buffer.resize(static_cast<uint32_t>(size));
		file.read(buffer.data(), size);
		if (!file.good())
			return false;

		return true;
	}

	template<typename FunctionType, typename... ArgumentsType>
	std::future<typename std::result_of<FunctionType(ArgumentsType...)>::type> RunAsync(FunctionType&& function, ArgumentsType&&... params)
	{
		return std::async(std::launch::async, std::forward<FunctionType>(function), std::forward<ArgumentsType>(params)...);
	}
}
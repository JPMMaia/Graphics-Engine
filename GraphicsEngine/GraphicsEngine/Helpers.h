#pragma once

#include <fstream>

namespace Helpers
{
	bool FileExists(const std::wstring& filename)
	{
		std::fstream fileStream(filename);

		return fileStream.good();
	}
}

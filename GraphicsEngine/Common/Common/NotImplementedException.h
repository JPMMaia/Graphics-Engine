#pragma once

#include <stdexcept>

namespace Common
{
	class NotImplementedException : public std::logic_error
	{
	public:
		explicit NotImplementedException();
	};
}

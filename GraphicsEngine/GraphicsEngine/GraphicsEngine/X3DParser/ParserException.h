#pragma once

#include <exception>
#include <string>

namespace GraphicsEngine
{
	class ParserException : std::runtime_error
	{
	public:
		ParserException(const std::wstring& message);

		static ParserException BuildAttributeNotFoundException(const std::wstring& attributeName);
		static ParserException BuildInvalidAttributeValueException(const std::wstring& attributeName);
	};
}
#pragma once

#include <exception>
#include <string>

namespace GraphicsEngine
{
	class DirectXException : public std::exception
	{
	public:
		DirectXException(HRESULT hr);

		char const* what() const override;

	private:
		void BuildErrorMessage(HRESULT hr);

	private:
		std::string m_errorMessage;
	};
}

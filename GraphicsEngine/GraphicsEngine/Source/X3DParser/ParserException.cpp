#include "../stdafx.h"
#include "ParserException.h"

using namespace GraphicsEngine;
using namespace std;

ParserException::ParserException(const wstring& message) :
	runtime_error(Helpers::WStringToString(message))
{
}

ParserException ParserException::BuildAttributeNotFoundException(const wstring& attributeName)
{
	return ParserException(L"Attribute " + attributeName + L" not found!");
}

ParserException ParserException::BuildInvalidAttributeValueException(const wstring& attributeName)
{
	return ParserException(L"Value of attribute " + attributeName + L" is null!");
}

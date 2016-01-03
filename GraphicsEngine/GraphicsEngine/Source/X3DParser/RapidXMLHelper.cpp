#include "RapidXMLHelper.h"

#include "ParserException.h"

using namespace GraphicsEngine;
using namespace DirectX;
using namespace rapidxml;
using namespace std;

void RapidXMLHelperClass::GetFirstAttribute(const xml_node<wchar_t>* node, const wstring& name, wstring& value, size_t nameSize, bool caseSensitive)
{
	const xml_attribute<wchar_t>* attribute = node->first_attribute(name.data(), nameSize, caseSensitive);
	if (!attribute)
		throw ParserException::BuildAttributeNotFoundException(name);

	auto valueString = attribute->value();
	if (!valueString)
		throw ParserException::BuildInvalidAttributeValueException(name);

	value.assign(valueString);
}
void RapidXMLHelperClass::GetFirstAttribute(const xml_node<wchar_t>* node, const wstring& name, bool& value, size_t nameSize, bool caseSensitive)
{
	wstring valueString;
	GetFirstAttribute(node, name, valueString, nameSize, caseSensitive);

	if (valueString == L"true")
		value = true;
	else if (valueString == L"false")
		value = false;
	else
		throw ParserException::BuildInvalidAttributeValueException(name);
}
void RapidXMLHelperClass::GetFirstAttribute(const xml_node<wchar_t>* node, const wstring& name, float& value, size_t nameSize, bool caseSensitive)
{
	wstring valueString;
	GetFirstAttribute(node, name, valueString, nameSize, caseSensitive);

	value = stof(valueString);
}
void RapidXMLHelperClass::GetFirstAttribute(const xml_node<wchar_t>* node, const wstring& name, XMFLOAT3& value, size_t nameSize, bool caseSensitive)
{
	wstring valueString;
	GetFirstAttribute(node, name, valueString, nameSize, caseSensitive);

	size_t size;
	value.x = stof(valueString, &size);
	value.y = stof(valueString.substr(size), &size);
	value.z = stof(valueString.substr(size), &size);
}
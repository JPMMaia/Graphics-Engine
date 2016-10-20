#pragma once

#include <string>
#include <list>
#include <DirectXMath.h>
#include <sstream>
#include <rapidxml/rapidxml.hpp>

namespace GraphicsEngine
{
	class RapidXMLHelperClass
	{
	public:
		static void GetFirstAttribute(const rapidxml::xml_node<wchar_t>* node, const std::wstring& name, std::wstring& value, size_t nameSize = 0U, bool caseSensitive = true);
		static void GetFirstAttribute(const rapidxml::xml_node<wchar_t>* node, const std::wstring& name, bool& value, size_t nameSize = 0U, bool caseSensitive = true);
		static void GetFirstAttribute(const rapidxml::xml_node<wchar_t>* node, const std::wstring& name, float& value, size_t nameSize = 0U, bool caseSensitive = true);
		static void GetFirstAttribute(const rapidxml::xml_node<wchar_t>* node, const std::wstring& name, DirectX::XMFLOAT3& value, size_t nameSize = 0U, bool caseSensitive = true);

		template<class T> static void GetFirstAttributeValues(const rapidxml::xml_node<wchar_t>* node, const std::wstring& name, std::list<T>& values, size_t nameSize = 0U, bool caseSensitive = true)
		{
			std::wstring valuesString;
			GetFirstAttribute(node, name, valuesString, nameSize, caseSensitive);

			std::wstringstream ss(valuesString);
			T value;

			values.clear();
			while (ss >> value)
				values.push_back(value);
		}
	};
}
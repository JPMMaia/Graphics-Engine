#include <rapidxml/rapidxml.hpp>

#include "X3DParser.h"
#include <rapidxml/rapidxml_utils.hpp>
#include <Source/Helpers.h>

using namespace GraphicsEngine;
using namespace std;
using namespace rapidxml;

X3DParserClass::X3DParserClass()
{
}
X3DParserClass::X3DParserClass(const std::wstring& filename)
{
	Parse(filename);
}

void X3DParserClass::Parse(const std::wstring& filename)
{
	file<wchar_t> file(Helpers::WStringToString(filename).data());
	xml_document<wchar_t> document;
	document.parse<0>(file.data());

	m_ShapeParser.Parse(m_Document.first_node(L"Shape"));
}

const X3DShapeParserClass::Shape& X3DParserClass::GetShape() const
{
	return m_ShapeParser.GetShape();
}
#include "stdafx.h"
#include "X3DParser.h"

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>
#include <Source/Helpers.h>

using namespace GraphicsEngine;
using namespace std;
using namespace rapidxml;

X3DParser::X3DParser()
{
}
X3DParser::X3DParser(const std::wstring& filename)
{
	Parse(filename);
}

void X3DParser::Parse(const std::wstring& filename)
{
	file<wchar_t> file(Helpers::WStringToString(filename).data());
	xml_document<wchar_t> document;
	document.parse<0>(file.data());

	m_shapeParser.Parse(document.first_node(L"Shape"));
}

const X3DShapeParser::Shape& X3DParser::GetShape() const
{
	return m_shapeParser.GetShape();
}
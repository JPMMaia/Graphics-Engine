#pragma once

#include <rapidxml/rapidxml.hpp>

#include "X3DShapeParser.h"

namespace GraphicsEngine
{
	class X3DParserClass
	{
	public:
		X3DParserClass();
		X3DParserClass(const std::wstring& filename);

		void Parse(const std::wstring& filename);

		const X3DShapeParserClass::Shape& GetShape() const;

	private:
		rapidxml::xml_document<wchar_t> m_Document;
		X3DShapeParserClass m_ShapeParser;
	};
}
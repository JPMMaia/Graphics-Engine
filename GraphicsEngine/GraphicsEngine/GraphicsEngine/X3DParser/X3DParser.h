#pragma once

#include <rapidxml/rapidxml.hpp>

#include "X3DShapeParser.h"

namespace GraphicsEngine
{
	class X3DParser
	{
	public:
		X3DParser();
		X3DParser(const std::wstring& filename);

		void Parse(const std::wstring& filename);

		const X3DShapeParser::Shape& GetShape() const;

	private:
		X3DShapeParser m_shapeParser;
	};
}
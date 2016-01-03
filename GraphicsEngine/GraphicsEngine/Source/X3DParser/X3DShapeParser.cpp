#include "X3DShapeParser.h"
#include "RapidXMLHelper.h"

#include <assert.h>

using namespace GraphicsEngine;
using namespace rapidxml;
using namespace std;

X3DShapeParser::X3DShapeParser()
{
}
X3DShapeParser::X3DShapeParser(const xml_node<wchar_t>* shapeNode)
{
	Parse(shapeNode);
}

void X3DShapeParser::Parse(const xml_node<wchar_t>* shapeNode)
{
	assert(shapeNode != nullptr);

	// Initialize Appearance:
	ParseAppearance(shapeNode->first_node(L"Appearance"), m_shape.appearance);

	// Initialize Indexed Triangle Set:
	ParseIndexedTriangleSet(shapeNode->first_node(L"IndexedTriangleSet"), m_shape.indexedTriangleSet);
}

const X3DShapeParser::Shape& X3DShapeParser::GetShape() const
{
	return m_shape;
}

void X3DShapeParser::ParseAppearance(const xml_node<wchar_t>* appearanceNode, X3DShapeParser::Appearance& appearance)
{
	assert(appearanceNode != nullptr);

	// Initialize Image Texture:
	ParseImageTexture(appearanceNode->first_node(L"ImageTexture"), appearance.imageTexture);

	// Initialize Material:
	ParseMaterial(appearanceNode->first_node(L"Material"), appearance.material);
}
void X3DShapeParser::ParseImageTexture(const xml_node<wchar_t>* imageTextureNode, ImageTexture& imageTexture)
{
	assert(imageTextureNode != nullptr);

	RapidXMLHelperClass::GetFirstAttribute(imageTextureNode, L"DEF", imageTexture.def);
	RapidXMLHelperClass::GetFirstAttribute(imageTextureNode, L"url", imageTexture.url);
}
void X3DShapeParser::ParseMaterial(const xml_node<wchar_t>* materialNode, X3DShapeParser::MaterialType& material)
{
	assert(materialNode != nullptr);

	RapidXMLHelperClass::GetFirstAttribute(materialNode, L"DEF", material.def);
	RapidXMLHelperClass::GetFirstAttribute(materialNode, L"diffuseColor", material.diffuseColor);
	RapidXMLHelperClass::GetFirstAttribute(materialNode, L"specularColor", material.specularColor);
	RapidXMLHelperClass::GetFirstAttribute(materialNode, L"emissiveColor", material.emissiveColor);
	RapidXMLHelperClass::GetFirstAttribute(materialNode, L"ambientIntensity", material.ambientInttensity);
	RapidXMLHelperClass::GetFirstAttribute(materialNode, L"shininess", material.shininess);
	RapidXMLHelperClass::GetFirstAttribute(materialNode, L"transparency", material.transparency);
}
void X3DShapeParser::ParseIndexedTriangleSet(const xml_node<wchar_t>* indexedTriangleSetNode, X3DShapeParser::IndexedTriangleSet& indexedTriangleSet)
{
	assert(indexedTriangleSetNode != nullptr);

	RapidXMLHelperClass::GetFirstAttribute(indexedTriangleSetNode, L"solid", indexedTriangleSet.solid);
	RapidXMLHelperClass::GetFirstAttribute(indexedTriangleSetNode, L"normalPerVertex", indexedTriangleSet.normalPerVertex);
	RapidXMLHelperClass::GetFirstAttributeValues<unsigned int>(indexedTriangleSetNode, L"index", indexedTriangleSet.index);

	ParseCoordinate(indexedTriangleSetNode->first_node(L"Coordinate"), indexedTriangleSet.coordinate);
	ParseNormal(indexedTriangleSetNode->first_node(L"Normal"), indexedTriangleSet.normal);
	ParseTextureCoordinate(indexedTriangleSetNode->first_node(L"TextureCoordinate"), indexedTriangleSet.textureCoordinate);
}
void X3DShapeParser::ParseCoordinate(const xml_node<wchar_t>* coordinateNode, list<float>& coordinate)
{
	assert(coordinateNode != nullptr);

	RapidXMLHelperClass::GetFirstAttributeValues<float>(coordinateNode, L"point", coordinate);
}
void X3DShapeParser::ParseNormal(const xml_node<wchar_t>* normalNode, list<float>& normal)
{
	assert(normalNode != nullptr);

	RapidXMLHelperClass::GetFirstAttributeValues<float>(normalNode, L"vector", normal);
}
void X3DShapeParser::ParseTextureCoordinate(const xml_node<wchar_t>* textureCoordinateNode, list<float>& textureCoordinate)
{
	assert(textureCoordinateNode != nullptr);

	RapidXMLHelperClass::GetFirstAttributeValues<float>(textureCoordinateNode, L"point", textureCoordinate);
}
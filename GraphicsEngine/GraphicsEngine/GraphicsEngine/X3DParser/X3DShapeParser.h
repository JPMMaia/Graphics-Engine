#pragma once

#include <string>
#include <list>
#include <DirectXMath.h>
#include <rapidxml/rapidxml.hpp>

namespace GraphicsEngine
{
	class X3DShapeParser
	{
	public:
		struct ImageTexture
		{
			std::wstring def;
			std::wstring url;
		};

		struct MaterialType
		{
			std::wstring def;
			DirectX::XMFLOAT3 diffuseColor;
			DirectX::XMFLOAT3 specularColor;
			DirectX::XMFLOAT3 emissiveColor;
			float ambientInttensity;
			float shininess;
			float transparency;
		};

		struct Appearance
		{
			ImageTexture imageTexture;
			MaterialType material;
		};

		struct IndexedTriangleSet
		{
			bool solid;
			bool normalPerVertex;
			std::list<unsigned int> index;
			std::list<float> coordinate;
			std::list<float> normal;
			std::list<float> textureCoordinate;
		};

		struct Shape
		{
			Appearance appearance;
			IndexedTriangleSet indexedTriangleSet;
		};

	public:
		X3DShapeParser();
		X3DShapeParser(const rapidxml::xml_node<wchar_t>* shapeNode);

		void Parse(const rapidxml::xml_node<wchar_t>* shapeNode);

		const Shape& GetShape() const;

	private:
		static void ParseAppearance(const rapidxml::xml_node<wchar_t>* appearanceNode, Appearance& appearance);
		static void ParseImageTexture(const rapidxml::xml_node<wchar_t>* imageTextureNode, ImageTexture& imageTexture);
		static void ParseMaterial(const rapidxml::xml_node<wchar_t>* materialNode, MaterialType& material);
		static void ParseIndexedTriangleSet(const rapidxml::xml_node<wchar_t>* indexedTriangleSetNode, IndexedTriangleSet& indexedTriangleSet);
		static void ParseCoordinate(const rapidxml::xml_node<wchar_t>* coordinateNode, std::list<float>& coordinate);
		static void ParseNormal(const rapidxml::xml_node<wchar_t>* normalNode, std::list<float>& normal);
		static void ParseTextureCoordinate(const rapidxml::xml_node<wchar_t>* textureCoordinateNode, std::list<float>& textureCoordinate);

	private:
		Shape m_shape;
	};
}
#pragma once

#include <string>
#include <list>
#include <DirectXMath.h>
#include <rapidxml/rapidxml.hpp>

namespace GraphicsEngine
{
	class X3DShapeParserClass
	{
	public:
		struct ImageTextureType
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

		struct AppearanceType
		{
			ImageTextureType imageTexture;
			MaterialType material;
		};

		struct IndexedTriangleSetType
		{
			bool solid;
			bool normalPerVertex;
			std::list<unsigned int> index;
			std::list<float> coordinate;
			std::list<float> normal;
			std::list<float> textureCoordinate;
		};

		struct ShapeType
		{
			AppearanceType appearance;
			IndexedTriangleSetType indexedTriangleSet;
		};

	public:
		X3DShapeParserClass();
		X3DShapeParserClass(const rapidxml::xml_node<wchar_t>* shapeNode);

		void Parse(const rapidxml::xml_node<wchar_t>* shapeNode);

		const ShapeType& GetShape() const;

	private:
		static void ParseAppearance(const rapidxml::xml_node<wchar_t>* appearanceNode, AppearanceType& appearance);
		static void ParseImageTexture(const rapidxml::xml_node<wchar_t>* imageTextureNode, ImageTextureType& imageTexture);
		static void ParseMaterial(const rapidxml::xml_node<wchar_t>* materialNode, MaterialType& material);
		static void ParseIndexedTriangleSet(const rapidxml::xml_node<wchar_t>* indexedTriangleSetNode, IndexedTriangleSetType& indexedTriangleSet);
		static void ParseCoordinate(const rapidxml::xml_node<wchar_t>* coordinateNode, std::list<float>& coordinate);
		static void ParseNormal(const rapidxml::xml_node<wchar_t>* normalNode, std::list<float>& normal);
		static void ParseTextureCoordinate(const rapidxml::xml_node<wchar_t>* textureCoordinateNode, std::list<float>& textureCoordinate);

	private:
		ShapeType m_shape;
	};
}
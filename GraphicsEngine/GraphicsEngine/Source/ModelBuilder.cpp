#include "stdafx.h"
#include "ModelBuilder.h"
#include "X3DParser/X3DParser.h"
#include "LightModel.h"

using namespace DirectX;
using namespace GraphicsEngine;
using namespace std;

ModelBuilder::ModelBuilder(TextureManager& textureManager) :
	m_textureManager(textureManager)
{
}

LightModel ModelBuilder::CreateFromX3D(ID3D11Device* d3dDevice, const std::wstring& x3dFilename) const
{
	// Parse x3d filename:
	X3DParser parser(x3dFilename);

	auto& shape = parser.GetShape();

	// Create vertices:
	auto& indexedTriangleSet = shape.indexedTriangleSet;
	auto& coordinates = indexedTriangleSet.coordinate;
	auto& normals = indexedTriangleSet.normal;
	auto& textureCoordinates = indexedTriangleSet.textureCoordinate;

	auto coordinate = coordinates.begin();
	auto normal = normals.begin();
	auto textureCoordinate = textureCoordinates.begin();

	unsigned int vertexCount = coordinates.size() / 3;
	vector<VertexPositionNormalTexture> vertices(vertexCount);
	for (unsigned int i = 0; i < vertexCount; i++)
	{
		VertexPositionNormalTexture& vertex = vertices[i];
		vertex.Position.x = *coordinate++;
		vertex.Position.y = *coordinate++;
		vertex.Position.z = *coordinate++;

		vertex.TextureCoordinate.x = *textureCoordinate++;
		vertex.TextureCoordinate.y = *textureCoordinate++;

		vertex.Normal.x = *normal++;
		vertex.Normal.y = *normal++;
		vertex.Normal.z = *normal++;
	}

	// Create indices:
	vector<uint32_t> indices(indexedTriangleSet.index.cbegin(), indexedTriangleSet.index.cend());

	// Create texture appearance:
	auto& appearance = shape.appearance;
	auto& material = appearance.material;
	auto& imageTexture = appearance.imageTexture;
	m_textureManager.Create(d3dDevice, imageTexture.def, imageTexture.url);
	auto alpha = 1.0f - material.transparency;
	TextureAppearance textureAppearance = 
	{
		Material(
			XMFLOAT4(material.ambientInttensity, material.ambientInttensity, material.ambientInttensity, alpha),
			XMFLOAT4(material.diffuseColor.x, material.diffuseColor.y, material.diffuseColor.z, alpha),
			XMFLOAT4(material.specularColor.x, material.specularColor.y, material.specularColor.z, alpha),
			material.shininess
			),
		m_textureManager[imageTexture.def]
	};

	Subset subset = { 0, indices.size() };

	return LightModel(d3dDevice, vertices, indices, {subset}, {textureAppearance});
}
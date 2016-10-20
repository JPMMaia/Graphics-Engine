#include "stdafx.h"
#include "GeometryGenerator2.h"

#include "X3DParser/X3DParser.h"

using namespace GraphicsEngine;

GeometryGenerator2::Vertex::Vertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT3& tangentU, const DirectX::XMFLOAT2& textureCoordinates):
	Position(position),
	Normal(normal),
	TangentU(tangentU),
	TextureCoordinates(textureCoordinates)
{
}

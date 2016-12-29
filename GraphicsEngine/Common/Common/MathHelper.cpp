#include "MathHelper.h"

using namespace DirectX;
using namespace GraphicsEngine;

void MathHelper::CalculateTangentBinormal(const XMFLOAT3& position0, const XMFLOAT3& position1, const XMFLOAT3& position2, const XMFLOAT2& textureCoordinates0, const XMFLOAT2 textureCoordidanates1, const XMFLOAT2 textureCoordinates2, XMFLOAT3& tangent, XMFLOAT3& binormal)
{
	// [ deltaU0 deltaV0 ] [ Tx Ty Tz ]     [ edge0.x edge0.y edge0.z ]
	// [                 ] [          ]  =  [                         ]
	// [ deltaU1 deltaV1 ] [ Bx By Bz ]     [ edge1.x edge1.y edge1.z ]

	// Set texture edge matrix:
	auto deltaUVMatrix = XMMatrixSet(
		textureCoordidanates1.x - textureCoordinates0.x, textureCoordidanates1.y - textureCoordinates0.y, 0.0f, 0.0f,
		textureCoordinates2.x - textureCoordinates0.x, textureCoordinates2.y - textureCoordinates0.y, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	// Calculate inverse of texture edge matrix:
	auto determinant = XMMatrixDeterminant(deltaUVMatrix);
	auto deltaUVInverseMatrix = XMMatrixInverse(&determinant, deltaUVMatrix);

	// Set edge matrix:
	auto edgeMatrix = DirectX::XMMatrixSet(
		position1.x - position0.x, position1.y - position0.y, position1.z - position0.z, 0.0f,
		position2.x - position0.x, position2.y - position0.y, position2.z - position0.z, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	// Calculate the tangent and binormal matrix:
	DirectX::XMFLOAT3X3 tangentBinormalMatrix;
	XMStoreFloat3x3(&tangentBinormalMatrix, XMMatrixMultiply(deltaUVInverseMatrix, edgeMatrix));

	tangent = DirectX::XMFLOAT3(tangentBinormalMatrix(0, 0), tangentBinormalMatrix(0, 1), tangentBinormalMatrix(0, 2));
	binormal = DirectX::XMFLOAT3(tangentBinormalMatrix(1, 0), tangentBinormalMatrix(1, 1), tangentBinormalMatrix(1, 2));
}

#pragma once

#include "Common/MathHelper.h"
#include "SubmeshGeometry.h"
#include "BufferTypes.h"
#include "GeometryGenerator.h"

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <unordered_map>
#include "VertexTypes.h"

namespace GraphicsEngine
{
	class D3DBase;

	struct MeshGeometry
	{
	public:
		std::string Name;
		std::unordered_map<std::string, SubmeshGeometry> Submeshes;
		VertexBuffer Vertices;
		IndexBuffer Indices;

	public:
		template<typename VertexType>
		static DirectX::BoundingBox CreateBoundingBoxFromMesh(const std::vector<VertexType>& vertices);

		static DirectX::BoundingBox CreateBoundingBoxFromMesh(const GeometryGenerator::MeshData& meshData);
	};

	template <typename VertexType>
	DirectX::BoundingBox MeshGeometry::CreateBoundingBoxFromMesh(const std::vector<VertexType>& vertices)
	{
		using namespace Common;
		using namespace DirectX;

		auto positionMin = XMVectorSet(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity, 0.0f);
		auto positionMax = XMVectorSet(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity, 0.0f);
		for (auto& vertex : vertices)
		{
			auto position = XMLoadFloat3(&vertex.Position);
			positionMin = XMVectorMin(positionMin, position);
			positionMax = XMVectorMax(positionMax, position);
		}

		BoundingBox bounds;
		XMStoreFloat3(&bounds.Center, 0.5f * (positionMin + positionMax));
		XMStoreFloat3(&bounds.Extents, 0.5f * (positionMax - positionMin));

		return bounds;
	}

	inline DirectX::BoundingBox MeshGeometry::CreateBoundingBoxFromMesh(const GeometryGenerator::MeshData& meshData)
	{
		return CreateBoundingBoxFromMesh(meshData.Vertices);
	}
}

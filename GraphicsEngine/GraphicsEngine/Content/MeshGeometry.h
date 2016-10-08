#pragma once

#include "SubmeshGeometry.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "MathHelper.h"

#include <memory>
#include <unordered_map>
#include <d3d12.h>
#include <DirectXCollision.h>

namespace GraphicsEngine
{
	struct MeshGeometry
	{
	public:
		std::string Name;
		std::unordered_map<std::string, SubmeshGeometry> DrawArgs;
		VertexBuffer Vertices;
		IndexBuffer Indices;

	public:
		MeshGeometry() = default;

		/// <sumary>
		/// We can free this memory after we finish upload to the GPU.
		/// </sumary>
		void DisposeUploaders();

		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

		static std::unique_ptr<MeshGeometry> LoadFromFile(const D3DBase& d3dBase, const std::string& name, const std::wstring& filename);

		template<typename VertexType, typename IndexType>
		static DirectX::BoundingBox CreateBoundingBoxFromMesh(const MeshGeometry& meshGeometry, const SubmeshGeometry& submesh)
		{
			using namespace DirectX;

			// Create vertex buffer:
			std::vector<VertexType> vertexBuffer(meshGeometry.Vertices.GetBufferByteSize() / sizeof(VertexType));
			auto vertexBlob = meshGeometry.Vertices.GetBufferCPU();
			CopyMemory(&vertexBuffer[0], vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize());

			// Create index buffer:
			std::vector<IndexType> indexBuffer(meshGeometry.Indices.GetBufferByteSize() / sizeof(IndexType));
			auto indexBlob = meshGeometry.Indices.GetBufferCPU();
			CopyMemory(&indexBuffer[0], indexBlob->GetBufferPointer(), indexBlob->GetBufferSize());

			auto positionMin = XMVectorSet(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity, 0.0f);
			auto positionMax = XMVectorSet(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity, 0.0f);
			for (size_t i = submesh.StartIndexLocation; i < submesh.IndexCount; ++i)
			{
				auto index = indexBuffer[i];
				auto& vertex = vertexBuffer[index];

				auto position = XMLoadFloat3(&vertex.Position);
				positionMin = XMVectorMin(positionMin, position);
				positionMax = XMVectorMax(positionMax, position);
			}

			BoundingBox bounds;
			XMStoreFloat3(&bounds.Center, 0.5f * (positionMin + positionMax));
			XMStoreFloat3(&bounds.Extents, 0.5f*(positionMax - positionMin));

			return bounds;
		}
	};
}

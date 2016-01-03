#pragma once

#include "Mesh.h"
#include "Subset.h"

struct ID3D11Device;

namespace GraphicsEngine
{
	template<typename VertexType, typename IndexType>
	struct Model
	{
		Model();
		Model(ID3D11Device* d3dDevice, const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices, const std::vector<Subset>& subsets);

		void Initialize(ID3D11Device* d3dDevice, const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices, const std::vector<Subset>& subsets);
		void Reset();

		Mesh Mesh;
		std::vector<VertexType> Vertices;
		std::vector<IndexType> Indices;
		std::vector<Subset> Subsets;
	};

	template <typename VertexType, typename IndexType>
	Model<VertexType, IndexType>::Model()
	{
	}

	template <typename VertexType, typename IndexType>
	Model<VertexType, IndexType>::Model(ID3D11Device* d3dDevice, const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices, const std::vector<Subset>& subsets)
	{
		Initialize(d3dDevice, vertices, indices, subsets);
	}

	template <typename VertexType, typename IndexType>
	void Model<VertexType, IndexType>::Initialize(ID3D11Device* d3dDevice, const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices, const std::vector<Subset>& subsets)
	{
		Mesh(d3dDevice, vertices, indices);
		Vertices.assign(vertices.cbegin(), vertices.cend());
		Indices.assign(indices.cbegin(), indices.cend());
		Subsets.assign(subsets.cbegin(), subsets.cend());
	}

	template <typename VertexType, typename IndexType>
	void Model<VertexType, IndexType>::Reset()
	{
		Subsets.clear();
		Indices.clear();
		Vertices.clear();
		Mesh.Reset();
	}
}
//***************************************************************************************
// Code extracted from d3dUtil.h by Frank Luna (C) 2015 All Rights Reserved.
//
// Mesh Geometry
//***************************************************************************************

#pragma once

#include "SubmeshGeometry.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <memory>
#include <unordered_map>

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
	};
}

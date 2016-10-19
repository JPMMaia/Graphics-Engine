#pragma once

#include "SubmeshGeometry.h"
#include "BufferTypes.h"

#include <unordered_map>

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
	};
}

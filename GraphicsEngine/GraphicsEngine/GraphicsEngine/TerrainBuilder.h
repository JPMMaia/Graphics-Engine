#pragma once

#include "D3DBase.h"
#include "RenderItem.h"
#include "GeometryGenerator2.h"

namespace GraphicsEngine
{
	class TerrainBuilder
	{
	public:
		static GeometryGenerator2::MeshData TerrainBuilder::CreateTerrain(const D3DBase& d3dBase, float width, float depth, uint32_t xCellCount, uint32_t zCellCount);
	};
}

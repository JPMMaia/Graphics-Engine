#pragma once

#include "D3DBase.h"
#include "RenderItem.h"
#include "GeometryGenerator2.h"

namespace GraphicsEngine
{
	class TerrainBuilder
	{
	public:
		static GeometryGenerator2::MeshData CreateTerrain(float width, float depth, uint32_t xCellCount, uint32_t zCellCount);

	public:
		static float s_defaultTerrainWidth;
		static float s_defaultTerrainDepth;
		static uint32_t s_defaultTerrainXCellCount;
		static uint32_t s_defaultTerrainZCellCount;
	};
}

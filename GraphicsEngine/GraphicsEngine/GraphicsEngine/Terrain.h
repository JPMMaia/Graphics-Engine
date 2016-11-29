#pragma once

#include "D3DBase.h"
#include "RenderItem.h"
#include "GeometryGenerator2.h"

namespace GraphicsEngine
{
	class Graphics;
	class IScene;
	class TextureManager;

	class Terrain
	{
	public:
		struct Description
		{
			float TerrainWidth;
			float TerrainDepth;
			uint32_t CellXCount;
			uint32_t CellZCount;
			std::wstring TiledDiffuseMapFilename;
			std::wstring TiledNormalMapFilename;
			std::wstring HeightMapFilename;
			uint32_t HeightMapWidth;
			uint32_t HeightMapHeight;
			float HeightMapFactor;
			float TiledTexelScale;
		};

	public:
		Terrain() = default;
		Terrain(const D3DBase& d3dBase, Graphics& graphics, TextureManager& textureManager, IScene& scene, const Description& description);

		std::vector<DirectX::XMFLOAT3> GenerateRandomPositions(SIZE_T count) const;

		float GetTerrainHeight(float x, float z) const;
		const Description& GetDescription() const;
		DirectX::XMFLOAT2 GetTexelSize() const;

	private:
		void CreateGeometry(const D3DBase& d3dBase, IScene& scene) const;
		void CreateMaterial(const D3DBase& d3dBase, TextureManager& textureManager, IScene& scene);
		void CreateRenderItem(const D3DBase& d3dBase, Graphics& graphics, IScene& scene);

		static GeometryGenerator2::MeshData CreateMeshData(float width, float depth, uint32_t xCellCount, uint32_t zCellCount);
		static void LoadRawHeightMap(const std::wstring& filename, uint32_t width, uint32_t height, float heightFactor, std::vector<float>& heightMap);

	public:
		Description m_description;
		std::vector<float> m_heightMap;
	};
}
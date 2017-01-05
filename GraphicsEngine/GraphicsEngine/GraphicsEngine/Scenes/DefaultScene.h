#pragma once

#include "GraphicsEngine/IScene.h"
#include "GraphicsEngine/MeshGeometry.h"
#include "GraphicsEngine/Terrain.h"
#include "Common/Timer.h"

#include <memory>
#include <unordered_map>
#include "GraphicsEngine/Material.h"


namespace GraphicsEngine
{
	class LightManager;
	class TextureManager;
	class D3DBase;
	class Graphics;

	class DefaultScene : public IScene
	{
	public:
		DefaultScene() = default;
		DefaultScene(Graphics* graphics, const D3DBase& d3dBase, TextureManager& textureManager, LightManager& lightManager);

		void Update(const Graphics& graphics, const Common::Timer& timer) override;

		void AddGeometry(std::unique_ptr<MeshGeometry>&& geometry) override;
		void AddMaterial(std::unique_ptr<Material>&& material) override;
		const Terrain& GetTerrain() const override;
		const DirectX::XMFLOAT4X4& GetGrassTransformMatrix() const;

		const std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& GetGeometries() const override;
		const std::unordered_map<std::string, std::unique_ptr<Material>>& GetMaterials() const override;

	private:
		void InitializeGeometry(const D3DBase& d3dBase);
		static void InitializeTextures(const D3DBase& d3dBase, TextureManager& textureManager);
		void InitializeMaterials(TextureManager& textureManager);
		void InitializeRenderItems(Graphics* graphics);
		void InitializeLights(LightManager& lightManager);

		void InitializeExternalModels(Graphics* graphics, const D3DBase& d3dBase, TextureManager& textureManager);
		void InitializeTerrain(Graphics* graphics, const D3DBase& d3dBase, TextureManager& textureManager);

	private:
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
		std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
		Terrain m_terrain;
		float m_grassRotation;
		float m_windDirection;
		DirectX::XMFLOAT4X4 m_grassTransformMatrix;
	};
}

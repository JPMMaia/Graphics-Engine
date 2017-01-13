#pragma once

#include "GraphicsEngine/IScene.h"
#include "GraphicsEngine/MeshGeometry.h"
#include "GraphicsEngine/Terrain.h"
#include "Common/Timer.h"

#include <memory>
#include <unordered_map>
#include "GraphicsEngine/Material.h"
#include "SceneBuilder.h"
#include "GraphicsEngine/ImmutableMeshGeometry.h"


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

		void AddImmutableGeometry(std::unique_ptr<ImmutableMeshGeometry>&& geometry) override;
		void AddBillboardGeometry(std::unique_ptr<BillboardMeshGeometry>&& geometry) override;
		void AddMaterial(std::unique_ptr<Material>&& material) override;
		const Terrain& GetTerrain() const override;
		Terrain& GetTerrain();
		const DirectX::XMFLOAT4X4& GetGrassTransformMatrix() const;

		const std::unordered_map<std::string, std::unique_ptr<ImmutableMeshGeometry>>& GetImmutableGeometries() const override;
		const std::unordered_map<std::string, std::unique_ptr<BillboardMeshGeometry>>& GetBillboardGeometries() const override;
		const std::unordered_map<std::string, std::unique_ptr<Material>>& GetMaterials() const override;
		void AddNormalInstances(Graphics* graphics, std::string name, const std::initializer_list<std::string>& renderItemNames, const std::vector<SceneBuilder::RenderItemInstanceData>& instancesData, DirectX::FXMMATRIX transformMatrix);
		void AddBillboardInstances(Graphics* graphics, std::string name, const std::initializer_list<std::string>& renderItemNames, const std::vector<SceneBuilder::RenderItemInstanceData>& instancesData);
		void AddTreeInstances(Graphics* graphics, const std::vector<SceneBuilder::RenderItemInstanceData>& instancesData);
		void AddGrassInstances(Graphics* graphics, const std::vector<SceneBuilder::RenderItemInstanceData>& instancesData);
		void RemoveLastInstance(Graphics* graphics, const std::string& itemName, const std::initializer_list<std::string>& renderItemNames);

	private:
		void InitializeTerrain(Graphics* graphics, const D3DBase& d3dBase, TextureManager& textureManager);
		void InitializeGeometry(const D3DBase& d3dBase);
		static void InitializeTextures(const D3DBase& d3dBase, TextureManager& textureManager);
		void InitializeMaterials(TextureManager& textureManager);
		void InitializeRenderItems(Graphics* graphics, const D3DBase& d3dBase, TextureManager& textureManager);
		void InitializeLights(LightManager& lightManager);

	private:
		bool m_initialized = false;
		std::unordered_map<std::string, std::unique_ptr<ImmutableMeshGeometry>> m_immutableGeometries;
		std::unordered_map<std::string, std::unique_ptr<BillboardMeshGeometry>> m_billboardGeometries;
		std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
		Terrain m_terrain;
		float m_grassRotation;
		float m_windDirection;
		DirectX::XMFLOAT4X4 m_grassTransformMatrix;
		
		SceneBuilder m_sceneBuilder;
		static std::wstring m_sceneBuilderFilename;
	};
}

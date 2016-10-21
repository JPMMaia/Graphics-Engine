#pragma once

#include "GraphicsEngine/IScene.h"
#include "GraphicsEngine/MeshGeometry.h"

#include <memory>
#include <unordered_map>
#include "GraphicsEngine/Material.h"

namespace GraphicsEngine
{
	class TextureManager;
	class D3DBase;
	class Graphics;

	class DefaultScene : public IScene
	{
	public:
		DefaultScene() = default;
		DefaultScene(Graphics* graphics, const D3DBase& d3dBase, TextureManager& textureManager);

		const std::unordered_map<std::string, std::unique_ptr<Material>>& GetMaterials() const override;

	private:
		void InitializeGeometry(const D3DBase& d3dBase);
		static void InitializeTextures(const D3DBase& d3dBase, TextureManager& textureManager);
		void InitializeMaterials(TextureManager& textureManager);
		void InitializeRenderItems(Graphics* graphics);

	private:
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
		std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
	};
}

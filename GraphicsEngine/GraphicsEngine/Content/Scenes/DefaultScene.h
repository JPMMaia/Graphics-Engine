#pragma once

#include "Content/IScene.h"
#include "Content/Material.h"
#include "Content/MeshGeometry.h"

#include <memory>
#include <unordered_map>

namespace GraphicsEngine
{
	class TextureManager;
	class D3DBase;
	class Graphics;

	class DefaultScene : public IScene
	{
	public:
		DefaultScene() = default;

		void AddTextures(TextureManager* pTextureManager) const;
		void Initialize(Graphics* graphics, const D3DBase& d3dBase, const TextureManager& textureManager);

		const std::unordered_map<std::string, std::unique_ptr<Material>>& GetMaterials() const override;

	private:
		void InitializeGeometry(const D3DBase& d3dBase);
		void InitializeMaterials(const TextureManager& textureManager);
		void InitializeRenderItems(Graphics* graphics, const D3DBase& d3dBase);

	private:
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
		std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
	};
}

#pragma once

#include "GraphicsEngine/IScene.h"
#include "GraphicsEngine/MeshGeometry.h"

namespace GraphicsEngine
{
	class TextureManager;
	class Graphics;

	class MirrorScene : public IScene
	{
	public:
		MirrorScene() = default;

		void AddTextures(TextureManager* pTextureManager) const;
		void Initialize(Graphics* graphics, const D3DBase& d3dBase, const TextureManager& textureManager);

		const std::unordered_map<std::string, std::unique_ptr<Material>>& GetMaterials() const override;

	private:
		void InitializeGeometry(const D3DBase& d3dBase);
		void InitializeMaterials(const TextureManager& textureManager);
		void InitializeRenderItems(Graphics* graphics);

	private:
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
		std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
	};
}

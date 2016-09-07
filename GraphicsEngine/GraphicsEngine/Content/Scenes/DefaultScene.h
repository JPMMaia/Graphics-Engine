#pragma once

#include "Content/IScene.h"
#include "Content/Material.h"
#include "Content/MeshGeometry.h"

#include <unordered_map>

namespace GraphicsEngine
{
	class TextureHeap;
	class D3DBase;
	class Graphics;

	class DefaultScene : public IScene
	{
	public:
		DefaultScene() = default;
		explicit DefaultScene(Graphics* graphics, const D3DBase& d3dBase);

		const std::unordered_map<std::string, std::unique_ptr<Material>>& GetMaterials() const override;

	private:
		void InitializeGeometry(const D3DBase& d3dBase);
		void InitializeTextures(Graphics* graphics, const D3DBase& d3dBase) const;
		void InitializeMaterials();
		void InitializeRenderItems(Graphics* graphics);

	private:
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
		std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
	};
}

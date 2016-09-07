#pragma once

#include "Content/IScene.h"
#include "Content/MeshGeometry.h"

namespace GraphicsEngine
{
	class Graphics;

	class MirrorScene : public IScene
	{
	public:
		MirrorScene() = default;
		explicit MirrorScene(Graphics* graphics, const D3DBase& d3dBase);

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

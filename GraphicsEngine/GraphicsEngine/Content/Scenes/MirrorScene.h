#pragma once

#include "Content/IScene.h"
#include "Content/MeshGeometry.h"

namespace GraphicsEngine
{
	class TextureHeap;
	class Graphics;

	class MirrorScene : public IScene
	{
	public:
		MirrorScene() = default;
		explicit MirrorScene(Graphics* graphics, const D3DBase& d3dBase, TextureHeap* textureHeap);

		const std::unordered_map<std::string, std::unique_ptr<Material>>& GetMaterials() const override;

	private:
		void InitializeGeometry(const D3DBase& d3dBase);
		void InitializeTextures(const D3DBase& d3dBase, TextureHeap* textureHeap);
		void InitializeMaterials();
		void InitializeRenderItems(Graphics* graphics);

	private:
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
		std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
		int m_textureHeapIndexOffset = 0;
	};
}

#pragma once

#include "Content/Material.h"
#include "Content/MeshGeometry.h"

#include <unordered_map>

namespace GraphicsEngine
{
	class D3DBase;
	class Graphics;

	class DefaultScene
	{
	public:
		DefaultScene() = default;
		explicit DefaultScene(Graphics* graphics, const D3DBase& d3dBase);

		const std::unordered_map<std::string, std::unique_ptr<Material>>& GetMaterials() const;

	private:
		void InitializeGeometry(Graphics* graphics, const D3DBase& d3dBase);
		void InitializeMaterials(Graphics* graphics, const D3DBase& d3dBase);
		void InitializeRenderItems(Graphics* graphics, const D3DBase& d3dBase);

	private:
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
		std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
	};
}

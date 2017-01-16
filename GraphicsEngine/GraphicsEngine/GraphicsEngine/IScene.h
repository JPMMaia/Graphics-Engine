#pragma once

#include "MeshGeometry.h"
#include "Material.h"
#include "Scenes/SceneBuilder.h"
#include "Common/Timer.h"

#include <memory>
#include <unordered_map>

namespace GraphicsEngine
{
	class Graphics;
	class Terrain;
	class ImmutableMeshGeometry;
	class BillboardMeshGeometry;

	class IScene
	{
	public:
		virtual ~IScene() = default;

		virtual void Update(const Graphics& graphics, const Common::Timer& timer) = 0;

		virtual void AddImmutableGeometry(std::unique_ptr<ImmutableMeshGeometry>&& geometry) = 0;
		virtual void AddBillboardGeometry(std::unique_ptr<BillboardMeshGeometry>&& geometry) = 0;
		virtual void AddMaterial(std::unique_ptr<Material>&& material) = 0;
		
		virtual const Terrain& GetTerrain() const = 0;
		virtual const std::unordered_map<std::string, std::unique_ptr<ImmutableMeshGeometry>>& GetImmutableGeometries() const = 0;
		virtual const std::unordered_map<std::string, std::unique_ptr<BillboardMeshGeometry>>& GetBillboardGeometries() const = 0;
		virtual const std::unordered_map<std::string, std::unique_ptr<Material>>& GetMaterials() const = 0;
	};
}

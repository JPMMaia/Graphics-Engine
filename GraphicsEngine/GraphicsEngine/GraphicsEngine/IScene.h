#pragma once

#include "MeshGeometry.h"
#include "Material.h"

#include <memory>
#include <unordered_map>

namespace GraphicsEngine
{
	class IScene
	{
	public:
		virtual ~IScene() = default;

		virtual void AddGeometry(std::unique_ptr<MeshGeometry>&& geometry) = 0;
		virtual void AddMaterial(std::unique_ptr<Material>&& material) = 0;

		virtual const std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& GetGeometries() const = 0;
		virtual const std::unordered_map<std::string, std::unique_ptr<Material>>& GetMaterials() const = 0;
	};
}

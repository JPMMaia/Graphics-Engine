#pragma once

#include "Material.h"

#include <memory>
#include <unordered_map>

namespace GraphicsEngine
{
	class IScene
	{
	public:
		virtual ~IScene()
		{
		}

		virtual const std::unordered_map<std::string, std::unique_ptr<Material>>& GetMaterials() const = 0;
	};
}

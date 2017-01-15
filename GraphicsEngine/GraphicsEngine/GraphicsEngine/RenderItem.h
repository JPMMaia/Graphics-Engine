#pragma once

#include "Material.h"
#include "OctreeCollider.h"

#include <unordered_set>

namespace GraphicsEngine
{
	class RenderItem
	{
	public:
		RenderItem() = default;
		virtual ~RenderItem() = default;

		virtual void Render(ID3D11DeviceContext* deviceContext) const = 0;
		virtual void RenderNonInstanced(ID3D11DeviceContext* deviceContext) const = 0;

		virtual void RemoveLastInstance() = 0;

		std::string GetName() const;
		void SetName(const std::string& name);
		Material* GetMaterial() const;
		void SetMaterial(Material* material);

	private:
		std::string m_name;
		Material* m_material = nullptr;
	};
}

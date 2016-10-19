#pragma once

#include "GraphicsEngine/IScene.h"
#include "GraphicsEngine/MeshGeometry.h"

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

		void Initialize(Graphics* graphics, const D3DBase& d3dBase);

	private:
		void InitializeGeometry(const D3DBase& d3dBase);
		void InitializeRenderItems(Graphics* graphics);

	private:
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
	};
}

#pragma once

#include "ISceneRenderer.h"

namespace GraphicsEngine
{
	class SceneRenderer : public ISceneRenderer
	{
	public:
		~SceneRenderer() override;

		void CreateDeviceDependentResources() override;
		void CreateWindowSizeDependentResources() override;
		void Update(const Timer& timer) override;
		void Render(const Timer& timer) override;
		void SaveState() override;
	};
}

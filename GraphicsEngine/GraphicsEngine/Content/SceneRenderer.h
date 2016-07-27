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
		void Update(float deltaSeconds) override;
		bool Render() override;
		void SaveState() override;
	};
}

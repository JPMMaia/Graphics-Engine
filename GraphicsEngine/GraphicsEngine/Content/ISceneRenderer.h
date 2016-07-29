#pragma once

#include "Timer.h"

namespace GraphicsEngine
{
	class ISceneRenderer
	{
	public:
		virtual ~ISceneRenderer()
		{
		}

		virtual void CreateDeviceDependentResources() = 0;
		virtual void CreateWindowSizeDependentResources() = 0;
		virtual void Update(const Timer& timer) = 0;
		virtual void Render(const Timer& timer) = 0;
		virtual void SaveState() = 0;
	};
}

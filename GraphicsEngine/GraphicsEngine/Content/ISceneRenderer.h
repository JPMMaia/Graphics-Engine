#pragma once

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
		virtual void Update(float deltaSeconds) = 0;
		virtual bool Render() = 0;
		virtual void SaveState() = 0;
	};
}

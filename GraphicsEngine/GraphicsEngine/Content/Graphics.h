#pragma once

#include "D3DBase.h"

namespace GraphicsEngine
{
	class Graphics
	{
	public:
		explicit Graphics(HWND outputWindow);
		
	private:
		D3DBase m_d3d;
	};
}

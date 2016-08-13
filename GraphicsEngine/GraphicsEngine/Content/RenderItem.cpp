#include "stdafx.h"
#include "RenderItem.h"
#include "Graphics.h"

using namespace GraphicsEngine;

RenderItem::RenderItem() :
	FramesDirtyCount(Graphics::GetFrameResourcesCount())
{
}

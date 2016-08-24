#include "stdafx.h"
#include "Material.h"
#include "Graphics.h"

using namespace GraphicsEngine;

Material::Material() :
	FramesDirtyCount(Graphics::GetFrameResourcesCount())
{
}

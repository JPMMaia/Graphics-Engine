#include "stdafx.h"
#include "RenderItem.h"
#include "Graphics.h"

using namespace GraphicsEngine;

std::string RenderItem::GetName() const
{
	return m_name;
}
void RenderItem::SetName(const std::string& name)
{
	m_name = name;
}
Material* RenderItem::GetMaterial() const
{
	return m_material;
}
void RenderItem::SetMaterial(GraphicsEngine::Material* material)
{
	m_material = material;
}

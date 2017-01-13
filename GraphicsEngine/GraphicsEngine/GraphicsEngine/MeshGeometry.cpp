#include "MeshGeometry.h"

using namespace GraphicsEngine;

void MeshGeometry::AddSubmesh(const std::string& name, SubmeshGeometry&& submesh)
{
	m_submeshes[name] = std::move(submesh);
}
void MeshGeometry::RemoveSubmesh(const std::string& name)
{
	m_submeshes.erase(name);
}
const SubmeshGeometry& MeshGeometry::GetSubmesh(const std::string& name)
{
	return m_submeshes.at(name);
}

std::string MeshGeometry::GetName() const
{
	return m_name;
}
void MeshGeometry::SetName(const std::string& name)
{
	m_name = name;
}
std::unordered_map<std::string, SubmeshGeometry> MeshGeometry::GetSubmeshes() const
{
	return m_submeshes;
}

#pragma once

#include "../RenderItem.h"

#include <string>

struct aiScene;

namespace GraphicsEngine
{
	class IScene;
	class Graphics;
	class TextureManager;

	class AssimpImporter
	{
	public:
		struct ImportInfo
		{
			std::unordered_map<std::string, std::string> MaterialByMesh;
		};

	public:
		void Import(Graphics* graphics, const D3DBase& d3dBase, TextureManager& textureManager, IScene* scene, const std::wstring& filename, ImportInfo& importInfo);

		static std::string BuildMeshName(const std::wstring& filename, const std::string& meshName);

	private:
		void ConvertToMeshData(Graphics* graphics, const D3DBase& d3dBase, TextureManager& textureManager, IScene* scene, const std::wstring& filename, const aiScene* importedScene, ImportInfo& importInfo);

		void AddGeometry(const D3DBase& d3dBase, const std::string& name, IScene* scene, const std::wstring& filename, const aiScene* importedScene, ImportInfo& importInfo);
		void AddMaterials(const D3DBase& d3dBase, TextureManager& textureManager, IScene* scene, const aiScene* importedScene);

	private:
		SIZE_T m_vertexCount = 0;
		SIZE_T m_indexCount = 0;
	};
}
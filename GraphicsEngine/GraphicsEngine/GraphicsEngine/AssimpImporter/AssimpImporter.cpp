#include "stdafx.h"
#include "AssimpImporter.h"
#include "Common/Helpers.h"
#include "../D3DBase.h"
#include "../IScene.h"
#include "../TextureManager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "GraphicsEngine/VertexTypes.h"

using namespace Common;
using namespace DirectX;
using namespace GraphicsEngine;

void AssimpImporter::Import(Graphics* graphics, const D3DBase& d3dBase, TextureManager& textureManager, IScene* scene, const std::wstring& filename, AssimpImporter::ImportInfo& importInfo)
{
	using namespace Assimp;

	// Create importer:
	Importer importer;

	// Import scene from file:
	auto flags =
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_SplitLargeMeshes |
		aiProcess_ConvertToLeftHanded |
		aiProcess_SortByPType;
	auto importedScene = importer.ReadFile(Helpers::WStringToString(filename), flags);

	// Check if the report failed:
	if (!importedScene)
	{
		auto errorMessage = L"Error while importing file: " + Helpers::StringToWString(importer.GetErrorString());
		ThrowEngineException(errorMessage.c_str());
	}

	return ConvertToMeshData(graphics, d3dBase, textureManager, scene, filename, importedScene, importInfo);
}

std::string AssimpImporter::BuildMeshName(const std::wstring& filename, const std::string& meshName)
{
	return Helpers::WStringToString(filename) + "/Mesh:" + meshName;
}

void AssimpImporter::ConvertToMeshData(Graphics* graphics, const D3DBase& d3dBase, TextureManager& textureManager, IScene* scene, const std::wstring& filename, const aiScene* importedScene, AssimpImporter::ImportInfo& importInfo)
{
	auto name = Helpers::WStringToString(filename);

	AddGeometry(d3dBase, name, scene, filename, importedScene, importInfo);
	AddMaterials(d3dBase, textureManager, scene, importedScene);
}

void AssimpImporter::AddGeometry(const D3DBase& d3dBase, const std::string& name, IScene* scene, const std::wstring& filename, const aiScene* importedScene, AssimpImporter::ImportInfo& importInfo)
{
	auto geometry = std::make_unique<MeshGeometry>();
	geometry->Name = name;

	// Count vertices and indices:
	m_vertexCount = 0;
	m_indexCount = 0;
	for (unsigned int meshIndex = 0; meshIndex < importedScene->mNumMeshes; ++meshIndex)
	{
		auto mesh = importedScene->mMeshes[meshIndex];

		m_vertexCount += mesh->mNumVertices;
		m_indexCount += mesh->mNumFaces * 3;
	}

	// Create vertices and indices vectors:
	std::vector<VertexTypes::DefaultVertexType> vertices;
	vertices.reserve(m_vertexCount);
	std::vector<uint32_t> indices;
	indices.reserve(m_indexCount);

	uint32_t baseVertexLocation = 0;
	uint32_t startIndexLocation = 0;
	for (unsigned int meshIndex = 0; meshIndex < importedScene->mNumMeshes; ++meshIndex)
	{
		auto mesh = importedScene->mMeshes[meshIndex];
		// Add vertices:
		{
			for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
			{
				VertexTypes::DefaultVertexType vertex;

				const auto& vertexPosition = mesh->mVertices[i];
				vertex.Position = XMFLOAT3(vertexPosition.x, vertexPosition.y, vertexPosition.z);

				const auto& normals = mesh->mNormals[i];
				vertex.Normal = XMFLOAT3(normals.x, normals.y, normals.z);
				if (mesh->mTextureCoords[0] == nullptr)
					OutputDebugString(std::wstring(L"Model " + Helpers::StringToWString(name) + L" loaded with no textures").c_str());
				else
				{
					const auto& textureCoordinates = mesh->mTextureCoords[0][i];
					vertex.TextureCoordinates = XMFLOAT2(textureCoordinates.x, textureCoordinates.y);
				
					const auto& tangentU = mesh->mTangents[i];
					vertex.TangentU = XMFLOAT3(tangentU.x, tangentU.y, tangentU.z);
				}
				vertices.push_back(vertex);
			}
		}

		// Add indices:
		{
			for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
			{
				const auto& face = mesh->mFaces[i];
				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}
		}

		// Create submesh:
		SubmeshGeometry submesh;
		submesh.IndexCount = static_cast<UINT>(indices.size()) - startIndexLocation;
		submesh.StartIndexLocation = startIndexLocation;
		submesh.BaseVertexLocation = baseVertexLocation;
		submesh.Bounds = MeshGeometry::CreateBoundingBoxFromMesh(vertices);
		geometry->Submeshes[mesh->mName.C_Str()] = std::move(submesh);

		// Assign material to mesh:
		aiString materialName;
		importedScene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_NAME, materialName);
		importInfo.MaterialByMesh.emplace(BuildMeshName(filename, mesh->mName.C_Str()), materialName.C_Str());

		// Increment location variables:
		baseVertexLocation += mesh->mNumVertices;
		startIndexLocation += mesh->mNumFaces * 3;
	}

	// Create vertex and index buffer:
	auto device = d3dBase.GetDevice();
	geometry->Vertices = VertexBuffer(device, vertices);
	geometry->Indices = IndexBuffer(device, indices);

	scene->AddGeometry(std::move(geometry));
}
void AssimpImporter::AddMaterials(const D3DBase& d3dBase, TextureManager& textureManager, IScene* scene, const aiScene* importedScene)
{
	for (unsigned int i = 0; i < importedScene->mNumMaterials; ++i)
	{
		auto importedMaterial = importedScene->mMaterials[i];

		auto renderMaterial = std::make_unique<Material>();

		// Set name:
		aiString name;
		importedMaterial->Get(AI_MATKEY_NAME, name);
		renderMaterial->Name = name.C_Str();

		// Set diffuse map:
		aiString texturePath;
		importedMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
		if(texturePath.length != 0)
		{
			auto texturePathStr = "Models/" + std::string(texturePath.C_Str());
			textureManager.Create(d3dBase.GetDevice(), texturePathStr, Helpers::StringToWString(texturePathStr));
			renderMaterial->DiffuseMap = &textureManager[texturePathStr];
		}

		// Set diffuse albedo:
		aiColor4D diffuseColor;
		importedMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
		renderMaterial->DiffuseAlbedo = XMFLOAT4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);

		// Set fresnel factor:
		float reflectivity;
		if (importedMaterial->Get(AI_MATKEY_REFLECTIVITY, reflectivity) == aiReturn_FAILURE)
			reflectivity = 0.2f;
		renderMaterial->FresnelR0 = XMFLOAT3(reflectivity, reflectivity, reflectivity);

		// Set roughness:
		float shininess;
		importedMaterial->Get(AI_MATKEY_SHININESS, shininess);
		renderMaterial->Roughness = 1.0f - (shininess / 256.0f);

		// Set material transform:
		renderMaterial->MaterialTransform = MathHelper::Identity4x4();

		// Add material to the scene:
		scene->AddMaterial(std::move(renderMaterial));
	}
}

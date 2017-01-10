#include "stdafx.h"
#include "SceneBuilder.h"
#include "Common/Helpers.h"

#include "nlohmann/json/json.hpp"

using namespace Common;
using namespace DirectX;
using namespace GraphicsEngine;
using namespace std;

std::vector<SceneBuilder::RenderItemInstanceData> SceneBuilder::s_renderItemsDataEmpty;

SceneBuilder::SceneBuilder(const std::wstring& filename) :
	m_filename(filename)
{
	if (Helpers::FileExists(filename))
		LoadFromFile();
}
SceneBuilder::~SceneBuilder()
{
	SaveToFile();
}

void SceneBuilder::AddRenderItemInstance(const std::string& renderItemID, const RenderItemInstanceData& data)
{
	auto location = m_renderItemsData.find(renderItemID);
	if (location == m_renderItemsData.end())
	{
		m_renderItemsData.emplace(renderItemID, std::vector<RenderItemInstanceData>(1, data));
		return;
	}

	location->second.push_back(data);
}
void SceneBuilder::RemoveLastRenderItemInstance(const std::string& renderItemID)
{
	auto location = m_renderItemsData.find(renderItemID);
	if (location == m_renderItemsData.end())
		return;

	if(!location->second.empty())
		location->second.pop_back();
}

const std::vector<SceneBuilder::RenderItemInstanceData>& SceneBuilder::GetRenderItemInstances(const std::string& renderItemID) const
{
	auto location = m_renderItemsData.find(renderItemID);
	if (location == m_renderItemsData.end())
		return s_renderItemsDataEmpty;

	return location->second;
}

void SceneBuilder::SaveToFile()
{
	std::ofstream fileStream(m_filename, ios::out);

	auto fileJson(nlohmann::json::array({}));

	for (const auto& pair : m_renderItemsData)
	{
		const auto& id = pair.first;
		const auto& renderItemInstancesData = pair.second;

		nlohmann::json instancesJson;
		for (const auto& instanceData : renderItemInstancesData)
		{
			instancesJson.push_back(
			{
				{ "Position", { instanceData.Position.x, instanceData.Position.y }, },
				{ "Rotation", { instanceData.Rotation.x, instanceData.Rotation.y, instanceData.Rotation.z } },
				{ "Scale", { instanceData.Scale.x, instanceData.Scale.y, instanceData.Scale.z } },
			}
			);
		}

		nlohmann::json renderItemJson =
		{
			{ "ID", id },
			{ "Instances", instancesJson },
		};

		fileJson.push_back(renderItemJson);
	}

	fileStream << fileJson;
}
void SceneBuilder::LoadFromFile()
{
	std::ifstream fileStream(m_filename, ios::in);

	nlohmann::json fileJson;
	fileStream >> fileJson;

	if (!fileJson.is_array())
		ThrowEngineException(L"Expecting an array");

	for (const auto& renderItemJson : fileJson)
	{
		auto id = renderItemJson.at("ID").get<std::string>();
		const auto& instancesJson = renderItemJson.at("Instances");

		auto& renderItemInstancesData = m_renderItemsData[id];
		for (const auto& instanceData : instancesJson)
		{
			RenderItemInstanceData renderItemInstanceData;

			const auto& positionJson = instanceData.at("Position");
			renderItemInstanceData.Position = XMFLOAT2(positionJson[0].get<float>(), positionJson[1].get<float>());

			const auto& rotationJson = instanceData.at("Rotation");
			renderItemInstanceData.Rotation = XMFLOAT3(rotationJson[0].get<float>(), rotationJson[1].get<float>(), rotationJson[2].get<float>());

			const auto& scaleJson = instanceData.at("Scale");
			renderItemInstanceData.Scale = XMFLOAT3(scaleJson[0].get<float>(), scaleJson[1].get<float>(), scaleJson[2].get<float>());

			renderItemInstancesData.push_back(renderItemInstanceData);
		}
	}
}

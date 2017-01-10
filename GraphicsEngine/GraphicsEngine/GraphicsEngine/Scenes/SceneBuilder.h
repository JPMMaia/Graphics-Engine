#pragma once

#include <unordered_map>

#include <DirectXMath.h>

namespace GraphicsEngine
{
	class SceneBuilder
	{
	public:
		struct RenderItemInstanceData
		{
			DirectX::XMFLOAT2 Position;
			DirectX::XMFLOAT3 Rotation;
			DirectX::XMFLOAT3 Scale;
		};

	public:
		SceneBuilder() = default;
		explicit SceneBuilder(const std::wstring& filename);
		~SceneBuilder();

		void AddRenderItemInstance(const std::string& renderItemID, const RenderItemInstanceData& data);
		void RemoveLastRenderItemInstance(const std::string& renderItemID);
		const std::vector<RenderItemInstanceData>& GetRenderItemInstances(const std::string& renderItemID) const;

		void SaveToFile();
		void LoadFromFile();

	private:
		std::wstring m_filename;
		std::unordered_map<std::string, std::vector<RenderItemInstanceData>> m_renderItemsData;
		static std::vector<RenderItemInstanceData> s_renderItemsDataEmpty;
	};
}

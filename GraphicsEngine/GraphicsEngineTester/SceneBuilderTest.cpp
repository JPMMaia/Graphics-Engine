#include "stdafx.h"
#include "CppUnitTest.h"

#include "GraphicsEngine/Scenes/SceneBuilder.h"
#include "Common/Helpers.h"

using namespace GraphicsEngine;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace GraphicsEngineTester
{
	TEST_CLASS(SceneBuilderTest)
	{
	public:
		TEST_METHOD(TestSceneBuilder1)
		{
			std::wstring filename(L"TestSceneBuilder.txt");

			{
				SceneBuilder sceneBuilder(filename);
			}

			{
				SceneBuilder sceneBuilder(filename);
			}

			std::remove(Common::Helpers::WStringToString(filename).c_str());
		}

		TEST_METHOD(TestSceneBuilder2)
		{
			std::wstring filename(L"TestSceneBuilder.txt");
			std::string grassID("Grass");
			std::vector<SceneBuilder::RenderItemInstanceData> instancesData(
				{
					{ { 0.0f, 0.0f } },
					{ { 0.0f, 1.0f } },
					{ { 2.0f, 1.0f } },
					{ { 0.0f, 0.0f } },
					{ { 0.0f, -1.0f } },
					{ { 0.0f, 0.0f } },
				}
				);
			
			{
				SceneBuilder sceneBuilder(filename);
				for (const auto& data : instancesData)
					sceneBuilder.AddRenderItemInstance(grassID, data);
				
				sceneBuilder.RemoveLastRenderItemInstance(grassID);
				sceneBuilder.AddRenderItemInstance(grassID, instancesData.back());
			}
			
			{
				SceneBuilder sceneBuilder(filename);
				const auto& loadedInstancesData = sceneBuilder.GetRenderItemInstances(grassID);
				
				Assert::AreEqual(instancesData.size(), loadedInstancesData.size());
				for(size_t i = 0; i < instancesData.size(); ++i)
				{
					const auto& originalPosition = instancesData[i].Position;
					const auto& loadedPosition = loadedInstancesData[i].Position;

					Assert::AreEqual(originalPosition.x, loadedPosition.x);
					Assert::AreEqual(originalPosition.y, loadedPosition.y);
				}
			}

			std::remove(Common::Helpers::WStringToString(filename).c_str());
		}
	};
}
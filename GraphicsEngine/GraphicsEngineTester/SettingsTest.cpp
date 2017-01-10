#include "stdafx.h"
#include "CppUnitTest.h"

#include "GraphicsEngine/SettingsManager.h"
#include "Common/EngineException.h"

using namespace GraphicsEngine;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace GraphicsEngineTester
{
    TEST_CLASS(SettingsTest)
    {
    public:
        TEST_METHOD(TestSettingsFile)
        {
			wstring filenameW = L"settings.conf";
	        auto manager = SettingsManager::Build(filenameW);

			Assert::IsTrue(Common::Helpers::FileExists(filenameW));
			Assert::IsFalse(manager.GetAdapterDescription().empty());
			Assert::IsTrue(manager.GetAdapterDedicatedVideoMemory() > 0);
        }
    };
}
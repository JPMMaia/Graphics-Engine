#include "stdafx.h"
#include "CppUnitTest.h"

#include <SettingsManager.h>
#include <Helpers.h>

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GraphicsEngineTester
{
    TEST_CLASS(SettingsTest)
    {
    public:
        TEST_METHOD(TestSettingsFile)
        {
			wstring filename = L"settings.conf";
			SettingsManager manager = SettingsManager::Build(filename);

			Assert::IsTrue(Helpers::FileExists(filename));
			Assert::IsFalse(manager.GetAdapterDescription().empty());
			Assert::IsTrue(manager.GetAdapterDedicatedVideoMemory() > 0);
        }
    };
}
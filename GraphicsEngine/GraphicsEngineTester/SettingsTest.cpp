#include "stdafx.h"
#include "CppUnitTest.h"

#include <SettingsManager.h>
#include <Helpers.h>

using namespace GraphicsEngine;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace rapidxml;
using namespace std;

namespace GraphicsEngineTester
{
    TEST_CLASS(SettingsTest)
    {
    public:
        TEST_METHOD(TestSettingsFile)
        {
			wstring filenameW = L"settings.conf";
			SettingsManager manager = SettingsManager::Build(filenameW);

			Assert::IsTrue(Helpers::FileExists(filenameW));
			Assert::IsFalse(manager.GetAdapterDescription().empty());
			Assert::IsTrue(manager.GetAdapterDedicatedVideoMemory() > 0);
        }
    };
}
#include "stdafx.h"
#include "CppUnitTest.h"

#include <Camera.h>

using namespace DirectX;
using namespace GraphicsEngine;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace GraphicsEngineTester
{
	TEST_CLASS(SettingsTest)
	{
	public:
		TEST_METHOD(TestCameraBuildBoundingFrustum)
		{
			auto aspectRatio = 16.0f / 9.0f;
			auto fovAngleY = XMConvertToRadians(35.0f);
			auto nearZ = 1.0f;
			auto farZ = 10.0f;

			auto projectionPlaneWidth = 2.0f * aspectRatio;
			auto projectionPlaneHeight = 2.0f;
			auto distanceToProjectionPlane = 1.0f / tan(fovAngleY / 2.0f);

			auto projectionMatrix = XMMatrixPerspectiveFovRH(fovAngleY, aspectRatio, nearZ, farZ);
			BoundingFrustum frustum;
			BoundingFrustum::CreateFromMatrix(frustum, projectionMatrix);
			
			// TODO try to use quartenions in the camera:
			auto x = -aspectRatio / distanceToProjectionPlane;
			auto y = 0.0f;
			auto z = 1;
		}
	};
}
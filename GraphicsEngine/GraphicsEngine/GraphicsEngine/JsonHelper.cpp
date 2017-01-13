#include "JsonHelper.h"

using namespace DirectX;
using namespace GraphicsEngine;

nlohmann::json __vectorcall JsonHelper::XMVectorToJson(DirectX::FXMVECTOR vector)
{
	XMFLOAT4 float4;
	XMStoreFloat4(&float4, vector);

	return{
		float4.x, float4.y, float4.z, float4.w
	};
}

DirectX::XMVECTOR __vectorcall JsonHelper::JsonToXMVector(nlohmann::json jsonArray)
{
	return XMVectorSet(jsonArray[0].get<float>(), jsonArray[1].get<float>(), jsonArray[2].get<float>(), jsonArray[3].get<float>());
}

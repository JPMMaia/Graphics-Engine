#pragma once

#include <nlohmann/json/json.hpp>
#include <DirectXMath.h>

namespace GraphicsEngine
{
	namespace JsonHelper
	{
		nlohmann::json XM_CALLCONV XMVectorToJson(DirectX::FXMVECTOR vector);
		DirectX::XMVECTOR XM_CALLCONV JsonToXMVector(nlohmann::json jsonArray);
	}
}

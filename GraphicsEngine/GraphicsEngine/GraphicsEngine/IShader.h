#pragma once

#include <d3d11_2.h>
#include <string>
#include <wrl/client.h>

namespace GraphicsEngine
{
	class IShader
	{
	public:
		virtual ~IShader() = default;
		virtual void Set(ID3D11DeviceContext* d3dDeviceContext) const = 0;

	protected:
		static Microsoft::WRL::ComPtr<ID3DBlob> CompileFromFile(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target);
	};
}

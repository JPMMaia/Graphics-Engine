#pragma once

#include "IShader.h"

namespace GraphicsEngine
{
	class HullShader : public IShader
	{
	public:
		HullShader() = default;
		HullShader(ID3D11Device* d3dDevice, const std::wstring& filename);
		HullShader(ID3D11Device* d3dDevice, const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target);

		void Initialize(ID3D11Device* d3dDevice, const void* shaderByteCode, SIZE_T byteCodeLength);
		void Reset();

		void Set(ID3D11DeviceContext* d3dDeviceContext) const override;

	private:
		Microsoft::WRL::ComPtr<ID3D11HullShader> m_hullShader;
	};
}

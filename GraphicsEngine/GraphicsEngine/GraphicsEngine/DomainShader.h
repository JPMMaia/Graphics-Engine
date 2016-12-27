#pragma once

#include "IShader.h"

namespace GraphicsEngine
{
	class DomainShader : public IShader
	{
	public:
		static const DomainShader s_null;

	public:
		DomainShader() = default;
		DomainShader(ID3D11Device* d3dDevice, const std::wstring& filename);
		DomainShader(ID3D11Device* d3dDevice, const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target);

		void Initialize(ID3D11Device* d3dDevice, const void* shaderByteCode, SIZE_T byteCodeLength);
		void Reset();

		void Set(ID3D11DeviceContext* d3dDeviceContext) const override;

	private:
		Microsoft::WRL::ComPtr<ID3D11DomainShader> m_domainShader;
	};
}

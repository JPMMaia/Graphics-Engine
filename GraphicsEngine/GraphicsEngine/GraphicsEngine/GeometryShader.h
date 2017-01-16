#pragma once

#include "IShader.h"

namespace GraphicsEngine
{
	class GeometryShader : public IShader
	{
	public:
		static const GeometryShader s_null;

	public:
		GeometryShader() = default;
		GeometryShader(ID3D11Device* d3dDevice, const std::wstring& filename);
		GeometryShader(ID3D11Device* d3dDevice, const std::wstring& filename, D3D11_SO_DECLARATION_ENTRY* streamOutputLayout, UINT streamOutputLayoutCount, UINT* bufferStrides, UINT bufferStridesCount, UINT rasterizedStream);
		GeometryShader(ID3D11Device* d3dDevice, const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target);
		GeometryShader(ID3D11Device* d3dDevice, const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target, D3D11_SO_DECLARATION_ENTRY* streamOutputLayout, UINT streamOutputLayoutCount, UINT* bufferStrides, UINT bufferStridesCount, UINT rasterizedStream);

		void Initialize(ID3D11Device* d3dDevice, const void* shaderByteCode, SIZE_T byteCodeLength);
		void Initialize(ID3D11Device* d3dDevice, const void* shaderByteCode, SIZE_T byteCodeLength, D3D11_SO_DECLARATION_ENTRY* streamOutputLayout, UINT streamOutputLayoutCount, UINT* bufferStrides, UINT bufferStridesCount, UINT rasterizedStream);
		void Reset();

		void Set(ID3D11DeviceContext* d3dDeviceContext) const override;

	private:
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;
	};
}

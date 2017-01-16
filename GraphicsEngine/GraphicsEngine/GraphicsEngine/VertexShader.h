#pragma once

#include "IShader.h"

#include <vector>

namespace GraphicsEngine
{
	class VertexShader : public IShader
	{
	public:
		static const VertexShader s_null;

	public:
		VertexShader() = default;
		VertexShader(ID3D11Device* d3dDevice, const std::wstring& filename, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDesc);
		VertexShader(ID3D11Device* d3dDevice, const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDesc);

		void Initialize(ID3D11Device* d3dDevice, const void* shaderByteCode, SIZE_T byteCodeLength, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDesc);
		void Reset();

		void Set(ID3D11DeviceContext* d3dDeviceContext) const override;

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	};
}
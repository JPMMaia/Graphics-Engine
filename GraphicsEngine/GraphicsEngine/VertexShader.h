#pragma once

namespace GraphicsEngine
{
	class VertexShader
	{
	public:
		VertexShader();
		VertexShader(ID3D11Device* d3dDevice, const std::wstring& filename, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDesc);

		bool Initialize(ID3D11Device* d3dDevice, const std::wstring& filename, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDesc);
		void Shutdown();

		void Set(ID3D11DeviceContext* d3dDeviceContext) const;

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	};
}
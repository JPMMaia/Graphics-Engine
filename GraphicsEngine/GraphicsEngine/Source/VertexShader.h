#pragma once

namespace GraphicsEngine
{
	class VertexShader
	{
	public:
		VertexShader();

		template<size_t ArraySize>
		VertexShader(ID3D11Device* d3dDevice, const std::wstring& filename, const std::array<D3D11_INPUT_ELEMENT_DESC, ArraySize>& vertexDesc);

		template<size_t ArraySize>
		void Initialize(ID3D11Device* d3dDevice, const std::wstring& filename, const std::array<D3D11_INPUT_ELEMENT_DESC, ArraySize>& vertexDesc);
		void Reset();

		void Set(ID3D11DeviceContext* d3dDeviceContext) const;

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	};

	template <size_t ArraySize>
	VertexShader::VertexShader(ID3D11Device* d3dDevice, const std::wstring& filename, const std::array<D3D11_INPUT_ELEMENT_DESC, ArraySize>& vertexDesc)
	{
		Initialize(d3dDevice, filename, vertexDesc);
	}

	template <size_t ArraySize>
	void VertexShader::Initialize(ID3D11Device* d3dDevice, const std::wstring& filename, const std::array<D3D11_INPUT_ELEMENT_DESC, ArraySize>& vertexDesc)
	{
		// Read data from vertex shader file:
		std::vector<char> fileData;
		Helpers::ReadData(filename, fileData);

		// Create vertex shader:
		DX::ThrowIfFailed(
			d3dDevice->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				m_vertexShader.GetAddressOf()
				)
			);

		// Create input layout:
		DX::ThrowIfFailed(
			d3dDevice->CreateInputLayout(
				vertexDesc.data(),
				vertexDesc.size(),
				&fileData[0],
				fileData.size(),
				m_inputLayout.GetAddressOf()
				)
			);
	}
}

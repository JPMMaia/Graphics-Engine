#pragma once

namespace GraphicsEngine
{
	class Shader
	{
	public:
		Shader() = default;
		explicit Shader(const std::wstring& filename);

		D3D12_SHADER_BYTECODE GetShaderBytecode() const;
		
	private:
		void LoadBinary(const std::wstring& filename);

	private:
		Microsoft::WRL::ComPtr<ID3DBlob> m_byteCode;
	};
}

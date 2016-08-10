#pragma once

namespace GraphicsEngine
{
	class Shader
	{
	public:
		explicit Shader(const std::wstring& filename);
		Shader(const Shader& rhs) = delete;
		void operator=(const Shader& rhs) = delete;
		
	private:
		void LoadBinary(const std::wstring& filename);

	private:
		Microsoft::WRL::ComPtr<ID3DBlob> m_byteCode;
	};
}

#pragma once

namespace GraphicsEngine
{
	class PixelShader
	{
	public:
		PixelShader();
		PixelShader(ID3D11Device* d3dDevice, const std::wstring& filename);

		bool Initialize(ID3D11Device* d3dDevice, const std::wstring& filename);
		void Shutdown();

		void Set(ID3D11DeviceContext* d3dDeviceContext) const;
		
	private:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	};
}

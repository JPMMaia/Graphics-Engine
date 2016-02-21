#pragma once

#include "BaseShader.h"

namespace GraphicsEngine
{
	class DomainShader : public BaseShader<DomainShader>
	{
	public:
		DomainShader();
		DomainShader(ID3D11Device* d3dDevice, const std::wstring& filename);

		void Reset();

		void Set(ID3D11DeviceContext* d3dDeviceContext) const;

	private:
		Microsoft::WRL::ComPtr<ID3D11DomainShader> m_domainShader;
	};
}

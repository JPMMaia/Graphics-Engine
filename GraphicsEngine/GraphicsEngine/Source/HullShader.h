#pragma once

#include "BaseShader.h"

namespace GraphicsEngine
{
	class HullShader : public BaseShader<HullShader>
	{
	public:
		HullShader();
		HullShader(ID3D11Device* d3dDevice, const std::wstring& filename);

		void Reset();

		void Set(ID3D11DeviceContext* d3dDeviceContext) const;

	private:
		Microsoft::WRL::ComPtr<ID3D11HullShader> m_hullShader;
	};
}

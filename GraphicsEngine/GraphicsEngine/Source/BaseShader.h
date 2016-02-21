#pragma once

#include <d3d11.h>

namespace GraphicsEngine
{
	template<typename Derived>
	class BaseShader
	{
	public:
		void Reset();

		void Set(ID3D11DeviceContext* d3dDeviceContext) const;
	};

	template <typename Derived>
	void BaseShader<Derived>::Reset()
	{
		static_cast<Derived*>(this)->Reset();
	}

	template <typename Derived>
	void BaseShader<Derived>::Set(ID3D11DeviceContext* d3dDeviceContext) const
	{
		static_cast<const Derived*>(this)->Set(d3dDeviceContext);
	}
}

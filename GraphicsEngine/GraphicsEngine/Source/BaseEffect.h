#pragma once

#include <d3d11_1.h>

namespace GraphicsEngine
{
	template<typename Derived>
	class BaseEffect
	{
		void Reset();

		void Set(ID3D11DeviceContext1* d3dDeviceContext) const;
	};

	template <typename Derived>
	void BaseEffect<Derived>::Reset()
	{
		static_cast<Derived*>(this)->Reset();
	}

	template <typename Derived>
	void BaseEffect<Derived>::Set(ID3D11DeviceContext1* d3dDeviceContext) const
	{
		static_cast<const Derived*>(this)->Set(d3dDeviceContext);
	}
}

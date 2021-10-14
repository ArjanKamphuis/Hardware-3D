#pragma once

#include <math.h>
#include <DirectXMath.h>

namespace ChiliMath
{
	template<typename T>
	constexpr T Gauss(T x, T sigma) noexcept
	{
		const auto ss = sigma * sigma;
		return (static_cast<T>(1.0) / sqrt(static_cast<T>(2.0) * static_cast<T>(DirectX::XM_2PI) * ss)) * exp(-(x * x) / (static_cast<T>(2.0) * ss));
	}
}

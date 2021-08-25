#include "NullPixelShader.h"

#include "BindableCodex.h"
#include "ChiliUtil.h"

namespace Bind
{
	NullPixelShader::NullPixelShader(const Graphics& gfx)
	{
	}

	void NullPixelShader::Bind(const Graphics& gfx) noexcept
	{
		GetDeviceContext(gfx)->PSSetShader(nullptr, nullptr, 0u);
	}

	std::shared_ptr<NullPixelShader> NullPixelShader::Resolve(const Graphics& gfx) noexcept(!IS_DEBUG)
	{
		return Codex::Resolve<NullPixelShader>(gfx);
	}

	std::wstring NullPixelShader::GenerateUID() noexcept
	{
		return ChiliUtil::ToWide(typeid(NullPixelShader).name());
	}

	std::wstring NullPixelShader::GetUID() const noexcept
	{
		return GenerateUID();
	}
}

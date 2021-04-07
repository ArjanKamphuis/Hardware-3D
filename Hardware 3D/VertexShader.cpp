#include "VertexShader.h"

#include "GraphicsThrowMacros.h"

VertexShader::VertexShader(const Graphics& gfx, const std::wstring& path)
{
	INFOMAN(gfx);

	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &mByteCode));
	GFX_THROW_INFO(GetDevice(gfx)->CreateVertexShader(mByteCode->GetBufferPointer(), mByteCode->GetBufferSize(), nullptr, &mShader));
}

void VertexShader::Bind(const Graphics& gfx) noexcept
{
	GetDeviceContext(gfx)->VSSetShader(mShader.Get(), nullptr, 0u);
}

ID3DBlob* VertexShader::GetByteCode() const noexcept
{
	return mByteCode.Get();
}
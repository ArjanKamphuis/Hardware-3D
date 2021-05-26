#include "VertexShader.h"

#include "BindableCodex.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	VertexShader::VertexShader(const Graphics& gfx, const std::wstring& path)
		: mPath(path)
	{
		INFOMAN(gfx);

		GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &mByteCode));
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
	std::shared_ptr<VertexShader> VertexShader::Resolve(const Graphics& gfx, const std::wstring& path)
	{
		return Codex::Resolve<VertexShader>(gfx, path);
	}

	std::wstring VertexShader::GenerateUID(const std::wstring& path)
	{
		const std::string name = typeid(VertexShader).name();
		return std::wstring{ name.begin(), name.end() } + L"#" + path;
	}

	std::wstring VertexShader::GetUID() const noexcept
	{
		return GenerateUID(mPath);
	}
}

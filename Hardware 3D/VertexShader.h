#pragma once

#include "Bindable.h"

namespace Bind
{
	class VertexShader : public Bindable
	{
	public:
		VertexShader(const Graphics& gfx, const std::wstring& path);
		void Bind(const Graphics& gfx) noexcept override;
		ID3DBlob* GetByteCode() const noexcept;

		static std::shared_ptr<Bindable> Resolve(const Graphics& gfx, const std::wstring& path);
		static std::wstring GenerateUID(const std::wstring& path);
		std::wstring GetUID() const noexcept override;

	private:
		std::wstring mPath;
		Microsoft::WRL::ComPtr<ID3DBlob> mByteCode;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> mShader;
	};
}

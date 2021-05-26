#pragma once

#include "Bindable.h"

namespace Bind
{
	class IndexBuffer : public Bindable
	{
	public:
		IndexBuffer(const Graphics& gfx, const std::vector<USHORT>& indices);
		IndexBuffer(const Graphics& gfx, const std::wstring& tag, const std::vector<USHORT>& indices);
		void Bind(const Graphics& gfx) noexcept override;
		UINT GetCount() const noexcept;

		static std::shared_ptr<IndexBuffer> Resolve(const Graphics& gfx, const std::wstring& tag, const std::vector<USHORT>& indices);
		template<typename ...Ignore>
		static std::wstring GenerateUID(const std::wstring& tag, Ignore&& ...ignore)
		{
			return GenerateUID_(tag);
		}
		std::wstring GetUID() const noexcept override;

	private:
		static std::wstring GenerateUID_(const std::wstring& tag);

	private:
		std::wstring mTag;
		UINT mCount;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mBuffer;
	};
}

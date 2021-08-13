#pragma once

#include <DirectXMath.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "ChiliWin.h"

namespace Dcb
{
	class LayoutElement
	{
	public:
		LayoutElement(size_t offset);

		virtual LayoutElement& operator[](const wchar_t* key);
		virtual const LayoutElement& operator[](const wchar_t* key) const;

		size_t GetOffsetBegin() const noexcept;
		virtual size_t GetOffsetEnd() const noexcept = 0;

		virtual size_t ResolveFloat3() const noexcept(!IS_DEBUG);

	private:
		size_t mOffset;
	};

	class Float3 : public LayoutElement
	{
	public:
		using LayoutElement::LayoutElement;
		size_t ResolveFloat3() const noexcept(!IS_DEBUG) override final;
		size_t GetOffsetEnd() const noexcept override final;
	};

	class Struct : public LayoutElement
	{
	public:
		using LayoutElement::LayoutElement;

		LayoutElement& operator[](const wchar_t* key) override final;
		const LayoutElement& operator[](const wchar_t* key) const override final;
		size_t GetOffsetEnd() const noexcept override final;

		template<typename T>
		void Add(const std::wstring& name);

	private:
		std::unordered_map<std::wstring, LayoutElement*> mMap;
		std::vector<std::unique_ptr<LayoutElement>> mElements;
	};

	class ElementRef
	{
	public:
		ElementRef(const LayoutElement* pLayout, std::byte* pBytes);

		ElementRef operator[](const wchar_t* key) noexcept(!IS_DEBUG);
		operator DirectX::XMFLOAT3&() noexcept(!IS_DEBUG);
		DirectX::XMFLOAT3& operator=(const DirectX::XMFLOAT3& rhs) noexcept(!IS_DEBUG);

	private:
		const LayoutElement* mLayout;
		std::byte* mBytes;
	};

	class Buffer
	{
	public:
		Buffer(const Struct& pLayout);
		ElementRef operator[](const wchar_t* key) noexcept(!IS_DEBUG);

	private:
		const Struct* mLayout;
		std::vector<std::byte> mBytes;
	};

	template<typename T>
	inline void Struct::Add(const std::wstring& name)
	{
		mElements.push_back(std::make_unique<T>(GetOffsetEnd()));
		if (!mMap.emplace(name, mElements.back().get()).second)
			assert(false);
	}
}

#pragma once

#include "ChiliWin.h"
#include <memory>
#include <string>
#include "ChiliException.h"

class Surface
{
public:
	class Color
	{
	public:
		constexpr Color() noexcept = default;
		constexpr Color(Color& other) noexcept
			: Value(other.Value)
		{}
		constexpr Color(UINT value) noexcept
			: Value(value)
		{}
		constexpr Color(UCHAR x, UCHAR r, UCHAR g, UCHAR b) noexcept
			: Value((x << 24u) | (r << 16u) | (g << 8) | b)
		{}
		constexpr Color(Color other, UCHAR x) noexcept
			: Value((x << 24u) | other.Value)
		{}
		Color& operator=(const Color& rhs) noexcept
		{
			Value = rhs.Value;
			return *this;
		}
		constexpr UCHAR GetX() const noexcept
		{
			return Value >> 24u;
		}
		constexpr UCHAR GetA() const noexcept
		{
			return GetX();
		}
		constexpr UCHAR GetR() const noexcept
		{
			return (Value >> 16u) & 0xFFu;
		}
		constexpr UCHAR GetG() const noexcept
		{
			return (Value >> 8u) & 0xFFu;
		}
		constexpr UCHAR GetB() const noexcept
		{
			return Value & 0xFFu;
		}
		void SetX(UCHAR x) noexcept
		{
			Value = (Value & 0xFFFFFFu) | (x << 24u);
		}
		void SetA(UCHAR a) noexcept
		{
			SetX(a);
		}
		void SetR(UCHAR r) noexcept
		{
			Value = (Value & 0xFFFFFFu) | (r << 16u);
		}
		void SetG(UCHAR g) noexcept
		{
			Value = (Value & 0xFFFFFFu) | (g << 8u);
		}
		void SetB(UCHAR b) noexcept
		{
			Value = (Value & 0xFFFFFFu) | b;
		}

	public:
		UINT Value = 0;;
	};
public:
	class Exception : public ChiliException
	{
	public:
		Exception(int line, const wchar_t* file, const std::wstring& info) noexcept;
		const wchar_t* GetType() const noexcept override;
		std::wstring GetErrorInfo() const noexcept;

	protected:
		void GenerateMessage() const noexcept override;

	private:
		const std::wstring mInfo;
	};

public:
	Surface(UINT width, UINT height) noexcept;
	Surface(const Surface& rhs) = delete;
	Surface& operator=(const Surface& rhs) = delete;
	Surface(Surface&& rhs) noexcept;
	Surface& operator=(Surface&& rhs) noexcept;

	static Surface FromFile(const std::wstring& filename);

	void Clear(Color fillValue) noexcept;
	void PutPixel(UINT x, UINT y, Color c) noexcept(!IS_DEBUG);
	Color GetPixel(UINT x, UINT y) const noexcept(!IS_DEBUG);

	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;

	Color* GetBufferPtr() noexcept;
	const Color* GetBufferPtr() const noexcept;
	const Color* GetBufferPtrConst() const noexcept;

	void Save(const std::wstring& filename) const;
	void Copy(const Surface& src) noexcept(!IS_DEBUG);

private:
	Surface(UINT width, UINT height, std::unique_ptr<Color[]> pBuffer) noexcept;

private:
	std::unique_ptr<Color[]> mBuffer;
	UINT mWidth = 0;
	UINT mHeight = 0;
};

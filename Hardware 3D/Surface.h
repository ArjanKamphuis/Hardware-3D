#pragma once

#include "ChiliWin.h"
#include <DirectXPackedVector.h>
#include <memory>
#include <string>
#include "ChiliException.h"

class Surface
{
public:
	using Color = DirectX::PackedVector::XMCOLOR;

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

	bool AlphaLoaded() const noexcept;

private:
	Surface(UINT width, UINT height, std::unique_ptr<Color[]> pBuffer, bool alphaLoaded = false) noexcept;

private:
	std::unique_ptr<Color[]> mBuffer;
	UINT mWidth = 0;
	UINT mHeight = 0;
	bool mAlphaLoaded = false;
};

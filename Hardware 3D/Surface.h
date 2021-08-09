#pragma once

#include "ChiliWin.h"
#include <DirectXPackedVector.h>
#include <optional>
#include <string>
#include "ChiliException.h"

#pragma warning(push)
#pragma warning(disable : 26812)
#include <DirectXTex.h>
#pragma warning(pop)

class Surface
{
public:
	using Color = DirectX::PackedVector::XMCOLOR;

public:
	class Exception : public ChiliException
	{
	public:
		Exception(int line, const wchar_t* file, const std::wstring& info, std::optional<HRESULT> hr = {}) noexcept;
		Exception(int line, const wchar_t* file, const std::wstring& filename, const std::wstring& info, std::optional<HRESULT> hr = {}) noexcept;
		const wchar_t* GetType() const noexcept override;
		std::wstring GetErrorInfo() const noexcept;

	protected:
		void GenerateMessage() const noexcept override;

	private:
		std::wstring mInfo;
		std::optional<HRESULT> mHR;
	};

public:
	Surface(size_t width, size_t height);
	Surface(const Surface& rhs) = delete;
	Surface& operator=(const Surface& rhs) = delete;
	Surface(Surface&& rhs) noexcept = default;
	Surface& operator=(Surface&& rhs) noexcept = default;

	static Surface FromFile(const std::wstring& filename);

	void Clear(Color fillValue) noexcept;
	void PutPixel(size_t x, size_t y, Color c) noexcept(!IS_DEBUG);
	Color GetPixel(size_t x, size_t y) const noexcept(!IS_DEBUG);

	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;

	Color* GetBufferPtr() noexcept;
	const Color* GetBufferPtr() const noexcept;
	const Color* GetBufferPtrConst() const noexcept;

	void Save(const std::wstring& filename) const;

	bool AlphaLoaded() const noexcept;

private:
	Surface(DirectX::ScratchImage scratch) noexcept;

private:
	static constexpr DXGI_FORMAT mFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
	DirectX::ScratchImage mScratch;
};

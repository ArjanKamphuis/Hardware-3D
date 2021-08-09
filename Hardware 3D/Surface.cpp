#include "Surface.h"

#include <cassert>
#include <filesystem>

using namespace DirectX;

#pragma warning(push)
#pragma warning(disable : 26812)

Surface::Surface(size_t width, size_t height)
{
	HRESULT hr = mScratch.Initialize2D(mFormat, width, height, 1u, 1u);
	if (FAILED(hr))
		throw Surface::Exception(__LINE__, __FILEW__, L"Failed to initialize ScratchImage", hr);
}

Surface Surface::FromFile(const std::wstring& filename)
{
	ScratchImage scratch;
	HRESULT hr = LoadFromWICFile(filename.c_str(), WIC_FLAGS_NONE, nullptr, scratch);
	if (FAILED(hr))
		throw Exception(__LINE__, __FILEW__, filename, L"Failed to load image", hr);

	if (scratch.GetImage(0u, 0u, 0u)->format != mFormat)
	{
		ScratchImage converted;
		hr = DirectX::Convert(*scratch.GetImage(0u, 0u, 0u), mFormat, TEX_FILTER_DEFAULT, TEX_THRESHOLD_DEFAULT, converted);
		if (FAILED(hr))
			throw Exception(__LINE__, __FILEW__, filename, L"Failed to convert image", hr);
		return Surface(std::move(converted));
	}

	return Surface(std::move(scratch));
}

void Surface::Clear(Color fillValue) noexcept
{
	const Image& imgData = *mScratch.GetImage(0u, 0u, 0u);
	for (size_t y = 0u; y < GetHeight(); y++)
	{
		Color* rowStart = reinterpret_cast<Color*>(imgData.pixels + imgData.rowPitch * y);
		std::fill(rowStart, rowStart + imgData.width, fillValue);
	}
}

void Surface::PutPixel(size_t x, size_t y, Color c) noexcept(!IS_DEBUG)
{
	assert(x >= 0);
	assert(x < GetWidth());
	assert(y >= 0);
	assert(y < GetHeight());
	const Image& imgData = *mScratch.GetImage(0u, 0u, 0u);
	reinterpret_cast<Color*>(&imgData.pixels[y * imgData.rowPitch])[x] = c;
}

Surface::Color Surface::GetPixel(size_t x, size_t y) const noexcept(!IS_DEBUG)
{
	assert(x >= 0);
	assert(x < GetWidth());
	assert(y >= 0);
	assert(y < GetHeight());
	const Image& imgData = *mScratch.GetImage(0u, 0u, 0u);
	return reinterpret_cast<Color*>(&imgData.pixels[y * imgData.rowPitch])[x];
}

UINT Surface::GetWidth() const noexcept
{
	return static_cast<UINT>(mScratch.GetMetadata().width);
}

UINT Surface::GetHeight() const noexcept
{
	return static_cast<UINT>(mScratch.GetMetadata().height);
}

Surface::Color* Surface::GetBufferPtr() noexcept
{
	return reinterpret_cast<Color*>(mScratch.GetPixels());
}

const Surface::Color* Surface::GetBufferPtr() const noexcept
{
	return const_cast<Surface*>(this)->GetBufferPtr();
}

const Surface::Color* Surface::GetBufferPtrConst() const noexcept
{
	return const_cast<Surface*>(this)->GetBufferPtr();
}

void Surface::Save(const std::wstring& filename) const
{
	const auto GetCodecID = [](const std::wstring& filename)
	{
		const std::filesystem::path path = filename;
		const std::wstring ext = path.extension().wstring();

		if (ext == L".png") return WIC_CODEC_PNG;
		if (ext == L".jpg") return WIC_CODEC_JPEG;
		if (ext == L".bmp") return WIC_CODEC_BMP;

		throw Exception(__LINE__, __FILEW__, filename, L"Image format not supported");
	};

	HRESULT hr = SaveToWICFile(*mScratch.GetImage(0u, 0u, 0u), WIC_FLAGS_NONE, GetWICCodec(GetCodecID(filename)), filename.c_str());
	if (FAILED(hr))
		throw Exception(__LINE__, __FILEW__, filename, L"Failed to save image", hr);
}

bool Surface::AlphaLoaded() const noexcept
{
	return !mScratch.IsAlphaAllOpaque();
}

Surface::Surface(ScratchImage scratch) noexcept
	: mScratch(std::move(scratch))
{
}

#pragma warning(pop)

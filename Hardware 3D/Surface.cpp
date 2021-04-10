#define FULL_WINTARD
#include "Surface.h"

#include <algorithm>
namespace Gdiplus
{
	using std::min;
	using std::max;
}

#include <cassert>
#include <gdiplus.h>
#include <sstream>

Surface::Surface(UINT width, UINT height) noexcept
	: mBuffer(std::make_unique<Color[]>(width * height)), mWidth(width), mHeight(height)
{
}

Surface::Surface(Surface&& rhs) noexcept
	: mWidth(rhs.mWidth), mHeight(rhs.mHeight), mBuffer(std::move(rhs.mBuffer))
{
}

Surface& Surface::operator=(Surface&& rhs) noexcept
{
	if (this != &rhs)
	{
		mWidth = rhs.mWidth;
		mHeight = rhs.mHeight;
		mBuffer = std::move(rhs.mBuffer);
		rhs.mBuffer = nullptr;
	}
	return *this;
}

Surface Surface::FromFile(const std::wstring& filename)
{
	Gdiplus::Bitmap bitmap(filename.c_str());
	if (bitmap.GetLastStatus() != Gdiplus::Status::Ok)
	{
		std::wstringstream ss;
		ss << L"Loading image [" << filename << L"]: failed to load.";
		throw Exception(__LINE__, __FILEW__, ss.str());
	}

	UINT width = bitmap.GetWidth();
	UINT height = bitmap.GetHeight();
	std::unique_ptr<Color[]> pBuffer = std::make_unique<Color[]>(static_cast<size_t>(width) * height);

	for (UINT y = 0; y < height; y++)
	{
		for (UINT x = 0; x < width; x++)
		{
			Gdiplus::Color c;
			bitmap.GetPixel(x, y, &c);
			pBuffer[static_cast<size_t>(y) * width + x] = c.GetValue();
		}
	}

	return Surface(width, height, std::move(pBuffer));
}

void Surface::Clear(Color fillValue) noexcept
{
	memset(mBuffer.get(), fillValue.Value, sizeof(Color) * mWidth * mHeight);
}

void Surface::PutPixel(UINT x, UINT y, Color c) noexcept(!IS_DEBUG)
{
	assert(x >= 0);
	assert(x < mWidth);
	assert(y >= 0);
	assert(y < mHeight);
	mBuffer[static_cast<size_t>(y) * mWidth + x] = c;
}

Surface::Color Surface::GetPixel(UINT x, UINT y) const noexcept(!IS_DEBUG)
{
	assert(x >= 0);
	assert(x < mWidth);
	assert(y >= 0);
	assert(y < mHeight);
	return mBuffer[static_cast<size_t>(y) * mWidth + x];
}

UINT Surface::GetWidth() const noexcept
{
	return mWidth;
}

UINT Surface::GetHeight() const noexcept
{
	return mHeight;
}

Surface::Color* Surface::GetBufferPtr() noexcept
{
	return mBuffer.get();
}

const Surface::Color* Surface::GetBufferPtr() const noexcept
{
	return mBuffer.get();
}

const Surface::Color* Surface::GetBufferPtrConst() const noexcept
{
	return mBuffer.get();
}

void Surface::Save(const std::wstring& filename) const
{
	auto GetEncoderClsid = [&filename](const wchar_t* format, CLSID& pClsid)->void
	{
		UINT num = 0;
		UINT size = 0;
		Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;

		Gdiplus::GetImageEncodersSize(&num, &size);
		if (size == 0)
		{
			std::wstringstream ss;
			ss << L"Saving surface to [" << filename << L"]: failed to get encoder; size == 0.";
			throw Exception(__LINE__, __FILEW__, ss.str());
		}

		pImageCodecInfo = static_cast<Gdiplus::ImageCodecInfo*>(malloc(size));
		if (pImageCodecInfo == nullptr)
		{
			std::wstringstream ss;
			ss << L"Saving surface to [" << filename << L"]: failed to get encoder; failed to allocate memory.";
			throw Exception(__LINE__, __FILEW__, ss.str());
		}

		if (Gdiplus::GetImageEncoders(num, size, pImageCodecInfo) != Gdiplus::Status::Ok)
		{
			std::wstringstream ss;
			ss << L"Saving surface to [" << filename << L"]: failed to get encoder; failed GetImageEncoders.";
			throw Exception(__LINE__, __FILEW__, ss.str());
		}
		
		// false positive, no idea how to fix pImageCodexInfo[j].MimeType
#pragma warning ( disable : 6385 )
#pragma warning ( push )
		for (UINT j = 0; j < num; j++)
		{
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
			{
				pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return;
			}
		}
#pragma warning ( pop )

		free(pImageCodecInfo);
		std::wstringstream ss;
		ss << L"Saving surface to [" << filename << L"]: failed to get encoder; failed to find matching encoder.";
		throw Exception(__LINE__, __FILEW__, ss.str());
	};

	CLSID bmpID;
	GetEncoderClsid(L"image/bmp", bmpID);

	Gdiplus::Bitmap bitmap(mWidth, mHeight, sizeof(Color) * mWidth, PixelFormat32bppARGB, reinterpret_cast<BYTE*>(mBuffer.get()));
	if (bitmap.Save(filename.c_str(), &bmpID, nullptr) != Gdiplus::Status::Ok)
	{
		std::wstringstream ss;
		ss << L"Saving surface to [" << filename << L"]: failed to save.";
		throw Exception(__LINE__, __FILEW__, ss.str());
	}
}

void Surface::Copy(const Surface& src) noexcept(!IS_DEBUG)
{
	assert(mWidth == src.mWidth);
	assert(mHeight == src.mHeight);
	memcpy(mBuffer.get(), src.mBuffer.get(), sizeof(Color) * mWidth * mHeight);
}

Surface::Surface(UINT width, UINT height, std::unique_ptr<Color[]> pBuffer) noexcept
	: mWidth(width), mHeight(height), mBuffer(std::move(pBuffer))
{
}

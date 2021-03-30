#pragma once

#include "ChiliWin.h"
#include <d3d11.h>
#include <wrl.h>
#include "ChiliException.h"
#include "DxgiInfoManager.h"

class Graphics
{
#pragma region Exceptions
public:
	class Exception : public ChiliException
	{
		using ChiliException::ChiliException;
	};

	class HrException : public Exception
	{
	public:
		HrException(int line, const wchar_t* file, HRESULT hr, std::vector<std::wstring> infoMsgs = {}) noexcept;
		const wchar_t* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::wstring GetErrorString() const noexcept;
		std::wstring GetErrorDescription() const noexcept;
		std::wstring GetErrorInfo() const noexcept;

	protected:
		void GenerateMessage() const noexcept override;

	private:
		HRESULT mHR;
		std::wstring mInfo;
	};

	class InfoException : public Exception
	{
	public:
		InfoException(int line, const wchar_t* file, std::vector<std::wstring> infoMsgs) noexcept;
		const wchar_t* GetType() const noexcept override;
		std::wstring GetErrorInfo() const noexcept;

	protected:
		void GenerateMessage() const noexcept override;

	private:
		std::wstring mInfo;
	};

	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const wchar_t* GetType() const noexcept override;

	private:
		std::wstring mReason;
	};
#pragma endregion

public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	void EndFrame();
	void ClearBuffer(float r, float g, float b) noexcept;

	void DrawTestTriangle();

private:
#if	defined(DEBUG) | defined(_DEBUG)
	DxgiInfoManager mInfoManager;
#endif
	Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mDeviceContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
};


#pragma once

#include "ChiliWin.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>
#include <wrl.h>
#include "ChiliException.h"
#include "DxgiInfoManager.h"

namespace Bind
{
	class Bindable;
}

class Graphics
{
	friend class Bind::Bindable;

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
	~Graphics() noexcept;

	void BeginFrame(float r = 0.0f, float g = 0.0f, float b = 0.0f);
	void EndFrame();

	void DrawIndexed(UINT count) const noexcept(!IS_DEBUG);

	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetCamera(DirectX::FXMMATRIX camera) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

	void OnResize(UINT width, UINT height);

	void ToggleImgui(bool state) noexcept;
	bool IsImguiEnabled() const noexcept;

private:
#if	defined(DEBUG) | defined(_DEBUG)
	mutable DxgiInfoManager mInfoManager;
#endif

	DirectX::XMMATRIX mProjection = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX mCamera = DirectX::XMMatrixIdentity();

	Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mDeviceContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDepthStencilView;

	static constexpr UINT mBufferCount = 2;
	bool mImguiEnabled = true;
};


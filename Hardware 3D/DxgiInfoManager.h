#pragma once

#include "ChiliWin.h"
#include <dxgidebug.h>
#include <string>
#include <vector>
#include <wrl.h>

class DxgiInfoManager
{
public:
	DxgiInfoManager();
	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;

	void Set() noexcept;
	std::vector<std::wstring> GetMessages() const;

private:
	unsigned long long mNext = 0u;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> mDxgiInfoQueue;
};


#pragma once

#include "DynamicConstantBuffer.h"

class LayoutCodex
{
public:
	static bool Has(const std::wstring& tag);
	static Dcb::Layout Load(const std::wstring& tag);
	static void Store(const std::wstring& tag, Dcb::Layout& layout);

private:
	static LayoutCodex& Get_();
	bool Has_(const std::wstring& tag) const;
	Dcb::Layout Load_(const std::wstring& tag) const;
	void Store_(const std::wstring& tag, Dcb::Layout& layout);

private:
	std::unordered_map<std::wstring, std::shared_ptr<Dcb::LayoutElement>> mMap;
};


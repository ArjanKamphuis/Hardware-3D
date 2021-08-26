#pragma once

#include <string>
#include <vector>
#include "ChiliTimer.h"

class PerfLog
{
private:
	class Entry
	{
	public:
		Entry(std::wstring s, float t);
		void WriteTo(std::wostream& out) const noexcept;

	private:
		std::wstring mLabel;
		float mTime;
	};

public:
	static void Start(const std::wstring& label = L"") noexcept;
	static void Mark(const std::wstring& label = L"") noexcept;

private:
	PerfLog() noexcept;
	~PerfLog();

	static PerfLog& Get_() noexcept;

	void Start_(const std::wstring& label = L"") noexcept;
	void Mark_(const std::wstring& label = L"") noexcept;
	void Flush_();

private:
	ChiliTimer mTimer;
	std::vector<Entry> mEntries;
};


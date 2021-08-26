#include "PerfLog.h"

#include <fstream>
#include <iomanip>

using namespace std::string_literals;

PerfLog::Entry::Entry(std::wstring s, float t)
    : mLabel(std::move(s)), mTime(t)
{
}

void PerfLog::Entry::WriteTo(std::wostream& out) const noexcept
{
    if (mLabel.empty())
        out << mTime * 1000.0f << L"ms\n"s;
    else
        out << L"["s << mLabel << L"] "s << mTime * 1000.0f << L"ms\n"s;
}

void PerfLog::Start(const std::wstring& label) noexcept
{
    Get_().Start_(label);
}

void PerfLog::Mark(const std::wstring& label) noexcept
{
    Get_().Mark_(label);
}

PerfLog::PerfLog() noexcept
{
    mEntries.reserve(200u);
}

PerfLog::~PerfLog()
{
    Flush_();
}

PerfLog& PerfLog::Get_() noexcept
{
    static PerfLog log;
    return log;
}

void PerfLog::Start_(const std::wstring& label) noexcept
{
    mEntries.emplace_back(label, 0.0f);
    mTimer.Mark();
}

void PerfLog::Mark_(const std::wstring& label) noexcept
{
    mEntries.emplace_back(label, mTimer.Peek());
}

void PerfLog::Flush_()
{
    std::wofstream file(L"perf.txt");
    file << std::setprecision(3) << std::fixed;
    for (const Entry& e : mEntries)
        e.WriteTo(file);
}

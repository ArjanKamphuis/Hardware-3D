#include "ChiliUtil.h"

#include <iomanip>
#include <sstream>

namespace ChiliUtil
{
    std::vector<std::wstring> TokenizeQuoted(const std::wstring& input)
    {
        std::wistringstream stream;
        stream.str(input);
        std::vector<std::wstring> tokens;
        std::wstring token;

        while (stream >> std::quoted(token))
            tokens.push_back(std::move(token));

        return tokens;
    }

    std::wstring ToWide(const std::string& narrow)
    {
        wchar_t wide[512] = {};
        mbstowcs_s(nullptr, wide, narrow.c_str(), _TRUNCATE);
        return wide;
    }

    std::string ToNarrow(const std::wstring& wide)
    {
        char narrow[512] = {};
        wcstombs_s(nullptr, narrow, wide.c_str(), _TRUNCATE);
        return narrow;
    }
}

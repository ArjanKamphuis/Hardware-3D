#pragma once

#include <string>
#include <vector>

namespace ChiliUtil
{
	std::vector<std::wstring> TokenizeQuoted(const std::wstring& input);
	
	std::wstring ToWide(const std::string& narrow);
	std::string ToNarrow(const std::wstring& wide);
};


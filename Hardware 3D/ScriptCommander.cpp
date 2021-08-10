#include "ScriptCommander.h"

#include <fstream>
#include "ChiliUtil.h"
#include "json.hpp"
#include "TexturePreprocessor.h"

using namespace ChiliUtil;

#define SCRIPT_ERROR(msg) Exception(__LINE__, __FILEW__, scriptPath, msg)

ScriptCommander::ScriptCommander(const std::vector<std::wstring>& args)
{
	if (args.size() >= 3 && args[1] == L"--commands")
	{
		const std::wstring& scriptPath = args[2];
		std::ifstream script(scriptPath);
		if (!script.is_open())
			throw SCRIPT_ERROR(L"Unable to open script file");

		nlohmann::json top;
		script >> top;
		if (!top.is_array())
			throw SCRIPT_ERROR(L"Top level should be array of commands");

		bool abort = false;
		for (const auto& j : top)
		{
			const std::string commandName = j["command"].get<std::string>();
			const auto params = j["params"];
			const std::wstring sourceWide = ToWide(params["source"].get<std::string>());

			if (commandName == "flip-y")
			{
				TexturePreprocessor::FlipYNormalMap(sourceWide, ToWide(params.value("dest", ToNarrow(sourceWide))));
				abort = true;
			}
			else if (commandName == "flip-y-obj")
			{
				TexturePreprocessor::FlipYAllNormalMapsInObj(sourceWide);
				abort = true;
			}
			else if (commandName == "validate-map")
			{
				TexturePreprocessor::ValidateNormalMap(sourceWide, params["min"], params["max"]);
				abort = true;
			}
			else
				throw SCRIPT_ERROR(L"Unknown command: " + ToWide(commandName));
		}

		if (abort)
			throw Completion(L"Command(s) completed successfully");
	}
}

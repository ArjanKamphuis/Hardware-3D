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
			const std::string commandName = j.at("command").get<std::string>();
			const auto params = j.at("params");

			if (commandName == "flip-y")
			{
				const auto source = params.at("source");
				TexturePreprocessor::FlipYNormalMap(ToWide(source), ToWide(params.value("dest", source)));
				abort = true;
			}
			else if (commandName == "flip-y-obj")
			{
				TexturePreprocessor::FlipYAllNormalMapsInObj(ToWide(params.at("source")));
				abort = true;
			}
			else if (commandName == "validate-map")
			{
				TexturePreprocessor::ValidateNormalMap(ToWide(params.at("source")), params.at("min"), params.at("max"));
				abort = true;
			}
			else
				throw SCRIPT_ERROR(L"Unknown command: " + ToWide(commandName));
		}

		if (abort)
			throw Completion(L"Command(s) completed successfully");
	}
}

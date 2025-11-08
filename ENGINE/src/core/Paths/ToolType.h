#pragma once
#include <string>

namespace ar
{
	enum class ToolType
	{
		K16,
		K08,
		K01,
		F10
	};

	std::string ToolTypeStr(ToolType t);
}
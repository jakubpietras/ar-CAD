#include "arpch.h"
#include "ToolType.h"

namespace ar
{

	std::string ToolTypeStr(ToolType t)
	{
		switch (t)
		{
		case ToolType::F10:
			return "f10";
		case ToolType::K01:
			return "k01";
		case ToolType::K08:
			return "k08";
		case ToolType::K16:
			return "k16";
		}
		return "error";
	}

}
#include "arpch.h"

namespace ar
{
	std::shared_ptr<spdlog::logger> Log::s_Logger;
	std::shared_ptr<spdlog::logger> Log::s_AppLogger;
	
	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_Logger = spdlog::stdout_color_mt("CORE");
		s_Logger->set_level(spdlog::level::trace);

		s_AppLogger = spdlog::stdout_color_mt("EDITOR");
		s_AppLogger->set_level(spdlog::level::trace);
	}
}
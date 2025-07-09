#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

namespace ar
{
	class Log
	{
	public:
		static void Init();
		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_Logger; }
		static std::shared_ptr<spdlog::logger>& GetAppLogger() { return s_AppLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_Logger;
		static std::shared_ptr<spdlog::logger> s_AppLogger;
	};
}

// ::ar (nothing preceding ::) means accessing GLOBAL namespace (just to avoid ambiguity)

// CORE: use for logging within the engine
#define AR_CORE_TRACE(...) ::ar::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define AR_CORE_INFO(...) ::ar::Log::GetCoreLogger()->info(__VA_ARGS__)
#define AR_CORE_WARN(...) ::ar::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define AR_CORE_ERROR(...) ::ar::Log::GetCoreLogger()->error(__VA_ARGS__)
#define AR_CORE_FATAL(...) ::ar::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// EDITOR: use for logging within the editor app
#define	AR_TRACE(...) ::ar::Log::GetAppLogger()->trace(__VA_ARGS__)
#define AR_INFO(...) ::ar::Log::GetAppLogger()->info(__VA_ARGS__)
#define AR_WARN(...) ::ar::Log::GetAppLogger()->warn(__VA_ARGS__)
#define AR_ERROR(...) ::ar::Log::GetAppLogger()->error(__VA_ARGS__)
#define AR_FATAL(...) ::ar::Log::GetAppLogger()->fatal(__VA_ARGS__)
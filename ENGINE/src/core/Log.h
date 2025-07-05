#pragma once
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace ar
{
	class Log
	{
	public:
		static void Init();
		static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }
	private:
		static std::shared_ptr<spdlog::logger> s_Logger;
	};
}

// ::ar (nothing preceding ::) means accessing GLOBAL namespace (just to avoid ambiguity)
#define AR_TRACE(...) ::ar::Log::GetLogger()->trace(__VA_ARGS__)
#define AR_INFO(...) ::ar::Log::GetLogger()->info(__VA_ARGS__)
#define AR_WARN(...) ::ar::Log::GetLogger()->warn(__VA_ARGS__)
#define AR_ERROR(...) ::ar::Log::GetLogger()->error(__VA_ARGS__)
#define AR_FATAL(...) ::ar::Log::GetLogger()->fatal(__VA_ARGS__)
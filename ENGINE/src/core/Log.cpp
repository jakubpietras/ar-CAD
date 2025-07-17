#include "arpch.h"
#include <iomanip>

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

    std::ostream& operator<<(std::ostream& os, const mat::Vec3& v) {
        return os << "[" << v.x << ", " << v.y << ", " << v.z << "]";
    }
    std::ostream& operator<<(std::ostream& os, const mat::Vec4& v) {
        return os << "[" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "]";
    }
    std::ostream& operator<<(std::ostream& os, const mat::Mat4& mat) {
        os << std::fixed << std::setprecision(3);
        for (int row = 0; row < 4; ++row) {
            os << "[ ";
            for (int col = 0; col < 4; ++col) {
                os << std::setw(8) << mat(row, col);
                if (col < 3) os << ", ";
            }
            os << " ]\n";
        }
        return os;
    }
    std::ostream& operator<<(std::ostream& os, const mat::Quat& q) {
        return os << "(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
    }
    std::ostream& operator<<(std::ostream& os, const Event& e)
    {
        return os << e.ToString();
    }
}

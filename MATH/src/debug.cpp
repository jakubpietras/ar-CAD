#include "debug.h"
#include <iomanip>

namespace ar
{
	std::ostream& operator<<(std::ostream& os, const Vec3& v)
	{
		return os << "[" << v.x << ", " << v.y << ", " << v.z << "]";
	}
	std::ostream& operator<<(std::ostream& os, const Vec4& v)
	{
		return os << "[" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "]";
	}
	std::ostream& operator<<(std::ostream& os, const Mat4& mat)
	{
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
	std::ostream& operator<<(std::ostream& os, const Quat& q)
	{
		return os << "(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
	}
}
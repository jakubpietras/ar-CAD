#include "trigonometry.h"

namespace ar
{
	float Radians(const float degrees)
	{
		auto pi = std::atanf(1) * 4;
		return degrees * pi / 180.0f;
	}
	float Degrees(const float radians)
	{
		const float pi = std::atanf(1) * 4;
		return radians * 180.0f / pi;
	}
}
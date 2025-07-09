#pragma once
#include <iostream>
#include "vector_types.h"
#include "matrix_types.h"
#include "quaternion.h"

namespace ar
{
	std::ostream& operator<<(std::ostream& os, const Vec3& v);
	std::ostream& operator<<(std::ostream& os, const Vec4& v);
	std::ostream& operator<<(std::ostream& os, const Mat4& mat);
	std::ostream& operator<<(std::ostream& os, const Quat& q);
}

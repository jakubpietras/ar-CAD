#pragma once
#include "vector_types.h"
#include <vector>

namespace ar::mat
{
	Vec3 EvaluateTorus(float smallRadius, float largeRadius, float u, float v);
	Vec3 EvaluateBezierPatch(const std::vector<Vec3>& controlPoints, float u, float v);
}
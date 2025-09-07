#pragma once
#include "vector_types.h"
#include <vector>

namespace ar::mat
{
	Vec3 DerivativeUTorus(float smallRadius, float largeRadius, float u, float v);
	Vec3 DerivativeVTorus(float smallRadius, float largeRadius, float u, float v);
	Vec3 DerivativeUBezierPatch(const std::vector<Vec3>& controlPoints, float u, float v);
	Vec3 DerivativeVBezierPatch(const std::vector<Vec3>& controlPoints, float u, float v);
}
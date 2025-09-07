#pragma once
#include "geometry.h"
#include "solvers.h"
#include <numbers>
#include <array>

namespace ar::mat 
{
	
	ar::mat::Vec3 EvaluateTorus(float smallRadius, float largeRadius, float u, float v)
	{
		float uu = u * 2 * std::numbers::pi;
		float vv = v * 2 * std::numbers::pi;

		// todo: * 2pi??
		// x = (R + r * cos(u)) * cos(v)
		auto x = (largeRadius + smallRadius * cos(uu)) * cos(vv);
		// y = (R + r * cos(u)) * sin(v)
		auto y = (largeRadius + smallRadius * cos(uu)) * sin(vv);
		// z = r * sin(u)
		auto z = smallRadius * sin(uu);
		return { x, y, z };
	}

	ar::mat::Vec3 EvaluateBezierPatch(const std::vector<Vec3>& controlPoints, float u, float v)
	{
		std::array<Vec3, 4> points, tmp;
		for (size_t row = 0; row < 4; row++)
		{
			int base = row * 4;
			tmp[0] = controlPoints[base];
			tmp[1] = controlPoints[base + 1];
			tmp[2] = controlPoints[base + 2];
			tmp[3] = controlPoints[base + 3];
			points[row] = CubicDeCasteljau(tmp, v);
		}
		return CubicDeCasteljau(points, u);
	}

}
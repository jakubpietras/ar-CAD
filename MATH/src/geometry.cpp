#pragma once
#include "geometry.h"
#include "solvers.h"
#include <numbers>
#include <array>

namespace ar::mat 
{
	
	ar::mat::Vec3 EvaluateTorus(float smallRadius, float largeRadius, float u, float v)
	{
		float twoPi = 2.f * std::numbers::pi;
		float theta = u * twoPi;
		float phi = v * twoPi;

		auto x = (largeRadius + smallRadius * cos(phi)) * cos(theta);
		auto y = smallRadius * sin(phi);
		auto z = (largeRadius + smallRadius * cos(phi)) * sin(theta);
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
			points[row] = CubicDeCasteljau(tmp, u);
		}
		return CubicDeCasteljau(points, v);
	}

}
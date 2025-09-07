#include "gradient.h"
#include <stdexcept>
#include "solvers.h"
#include <numbers>
#include <array>

namespace ar
{
	ar::mat::Vec3 mat::DerivativeUTorus(float smallRadius, float largeRadius, float u, float v)
	{
		float uu = u * 2 * std::numbers::pi;
		float vv = v * 2 * std::numbers::pi;

		// x = (R + r * cos(u)) * cos(v)
		auto dxdu = -smallRadius * cos(vv) * sin(uu);
		// y = (R + r * cos(u)) * sin(v)
		auto dydu = -smallRadius * sin(vv) * sin(uu);
		// z = r * sin(u)
		auto dzdu = smallRadius * cos(uu);
		return { dxdu, dydu, dzdu };
	}

	ar::mat::Vec3 mat::DerivativeVTorus(float smallRadius, float largeRadius, float u, float v)
	{
		float uu = u * 2 * std::numbers::pi;
		float vv = v * 2 * std::numbers::pi;

		// x = (R + r * cos(u)) * cos(v)
		auto dxdv = -(smallRadius * cos(uu) + largeRadius) * sin(vv);
		// y = (R + r * cos(u)) * sin(v)
		auto dydv = (smallRadius * cos(uu) + largeRadius) * cos(vv);
		// z = r * sin(u)
		auto dzdv = 0.f;
		return { dxdv, dydv, dzdv };
	}

	ar::mat::Vec3 mat::DerivativeUBezierPatch(const std::vector<Vec3>& controlPoints, float u, float v)
	{
		// P(u,v) = Bi(u) * Bj(v) * Pij (sum)
		// dPdu = Bi'(u) * Bj(v) * Pij
		// controlPoints is always 4x4

		if (controlPoints.size() != 16)
			throw std::runtime_error("Incorrect number of control points in a single patch");

		std::array<mat::Vec3, 4> points, tmp;
		for (size_t row = 0; row < 4; row++)
		{
			size_t base = row * 4;
			tmp[0] = controlPoints[base];
			tmp[1] = controlPoints[base + 1];
			tmp[2] = controlPoints[base + 2];
			tmp[3] = controlPoints[base + 3];
			points[row] = CubicBernsteinDerivative(tmp, u);
		}
		return CubicDeCasteljau(points, v);
	}

	ar::mat::Vec3 mat::DerivativeVBezierPatch(const std::vector<Vec3>& controlPoints, float u, float v)
	{
		// P(u,v) = Bi(u) * Bj(v) * Pij (sum)
		// dPdu = Bi(u) * Bj'(v) * Pij
		// controlPoints is always 4x4

		if (controlPoints.size() != 16)
			throw std::runtime_error("Incorrect number of control points in a single patch");
		
		std::array<mat::Vec3, 4> points, tmp;
		for (size_t col = 0; col < 4; col++)
		{
			tmp[0] = controlPoints[col];
			tmp[1] = controlPoints[col + 1 * 4];
			tmp[2] = controlPoints[col + 2 * 4];
			tmp[3] = controlPoints[col + 3 * 4];
			points[col] = CubicBernsteinDerivative(tmp, v);
		}
		return CubicDeCasteljau(points, u);
	}

}
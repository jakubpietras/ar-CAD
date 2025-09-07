#include "gradient.h"
#include <stdexcept>
#include "solvers.h"
#include <numbers>

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
		auto dxdu = -(smallRadius * cos(uu) + largeRadius) * sin(vv);
		// y = (R + r * cos(u)) * sin(v)
		auto dydu = (smallRadius * cos(uu) + largeRadius) * cos(vv);
		// z = r * sin(u)
		auto dzdu = 0.f;
		return { dxdu, dydu, dzdu };
	}

	ar::mat::Vec3 mat::DerivativeUBezierPatch(const std::vector<Vec3>& controlPoints, float u, float v)
	{
		// P(u,v) = Bi(u) * Bj(v) * Pij (sum)
		// dPdu = Bi'(u) * Bj(v) * Pij
		// controlPoints is always 4x4

		if (controlPoints.size() != 16)
			throw std::runtime_error("Incorrect number of control points in a single patch");

		std::vector<Vec3> pointsV(4);
		for (size_t row = 0; row < 4; row++)
		{
			size_t base = row * 4;
			pointsV[row] = BernsteinDerivative(std::vector{ controlPoints[base], controlPoints[base + 1], controlPoints[base + 2], controlPoints[base + 3] }, u);
		}
		return DeCasteljau(pointsV, v);
	}

	ar::mat::Vec3 mat::DerivativeVBezierPatch(const std::vector<Vec3>& controlPoints, float u, float v)
	{
		// P(u,v) = Bi(u) * Bj(v) * Pij (sum)
		// dPdu = Bi(u) * Bj'(v) * Pij
		// controlPoints is always 4x4

		if (controlPoints.size() != 16)
			throw std::runtime_error("Incorrect number of control points in a single patch");

		std::vector<Vec3> pointsU(4);
		for (size_t col = 0; col < 4; col++)
		{
			pointsU[col] = BernsteinDerivative(std::vector{ controlPoints[col], controlPoints[col + 4], controlPoints[col + 8], controlPoints[col + 12] }, v);
		}
		return DeCasteljau(pointsU, u);
	}

}
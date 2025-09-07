#pragma once
#include <vector>
#include "vector_types.h"
#include <type_traits>
#include <stdexcept>

namespace ar
{
    namespace mat
    {
		std::vector<float> SolveTridiagonal(
			const std::vector<float>& lower,
			const std::vector<float>& main,
			const std::vector<float>& upper,
			const std::vector<float>& rhs);
		std::vector<Vec3> SolveTridiagonal(
			const std::vector<float>& lower,
			const std::vector<float>& main,
			const std::vector<float>& upper,
			const std::vector<Vec3>& rhs);
		
		template <typename T, typename U>
		auto Lerp(const T& a, const T& b, const U& t)
			-> decltype(a + (b - a) * t)
		{
			return a + (b - a) * t;
		}

		template <typename T, typename U>
		T DeCasteljau(std::vector<T> controlPoints, U t)
		{
			if (controlPoints.empty())
				throw std::runtime_error("Control points cannot be empty");

			std::vector<T> points = controlPoints;

			while (points.size() > 1) {
				std::vector<T> nextLevel;
				for (size_t i = 0; i < points.size() - 1; ++i) {
					nextLevel.push_back(Lerp(points[i], points[i + 1], t));
				}
				points = std::move(nextLevel);
			}

			return points[0];
		}

		template <typename T, typename U>
		T BernsteinDerivative(std::vector<T> controlPoints, U t)
		{
			if (controlPoints.empty())
				throw std::runtime_error("Control points cannot be empty");

			if (controlPoints.size() == 1)
				return T{};

			std::vector<T> derivativeControlPoints;
			derivativeControlPoints.reserve(controlPoints.size() - 1);

			int n = static_cast<int>(controlPoints.size()) - 1;

			for (size_t i = 0; i < controlPoints.size() - 1; ++i) 
			{
				derivativeControlPoints.push_back(n * (controlPoints[i + 1] - controlPoints[i]));
			}
			return DeCasteljau(derivativeControlPoints, t);
		}

		template <typename T, typename U>
		T CubicDeCasteljau(const std::array<T, 4>& controlPoints, U t)
		{
			std::array<T, 3> level1 = {
				Lerp(controlPoints[0], controlPoints[1], t),
				Lerp(controlPoints[1], controlPoints[2], t),
				Lerp(controlPoints[2], controlPoints[3], t)
			};
			std::array<T, 2> level2 = {
				Lerp(level1[0], level1[1], t),
				Lerp(level1[1], level1[2], t)
			};
			return Lerp(level2[0], level2[1], t);
		}

		template <typename T, typename U>
		T CubicBernsteinDerivative(const std::array<T, 4>& controlPoints, U t)
		{
			std::array<T, 3> derivativeControlPoints = {
				3 * (controlPoints[1] - controlPoints[0]),
				3 * (controlPoints[2] - controlPoints[1]),
				3 * (controlPoints[3] - controlPoints[2])
			};
			std::array<T, 2> level1 = {
				Lerp(derivativeControlPoints[0], derivativeControlPoints[1], t),
				Lerp(derivativeControlPoints[1], derivativeControlPoints[2], t)
			};
			return Lerp(level1[0], level1[1], t);
		}

    }
}
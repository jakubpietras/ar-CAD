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

    }
}
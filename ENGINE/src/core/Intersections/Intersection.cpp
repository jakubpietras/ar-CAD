#include "arpch.h"
#include "Intersection.h"
#include <core/Scene/Components.h>
#include "core/Utils/GeneralUtils.h"
#include <algorithm>
#include <ranges>
#include <limits>
#include "core/Utils/SurfaceUtils.h"
#include "core/Scene/DebugRenderer.h"
#include "core/Utils/Parametric.h"
#include "algorithm/conjugateGradient.h"
#include "algorithm/newton.h"

namespace ar
{
	ar::mat::Vec3d Intersection::FindStartingPoint(ar::Entity firstObject, ar::Entity secondObject)
	{
		const size_t samples = 10;
		bool selfIntersection = IsSelfIntersection(firstObject, secondObject);
		auto params = GenerateUVPairs(samples, false);
		auto first = Parametric::Create(firstObject);
		auto second = Parametric::Create(secondObject);
		auto cg = mat::ConjugateGradientSD(first, second);

		float bestDistance = std::numeric_limits<float>::max();
		mat::Vec4d bestGuess;

		for (auto& pair : params)
		{
			auto optimizedParams = cg.Minimize({ pair.x, pair.y, pair.z, pair.w });
			auto s1 = first->Evaluate(optimizedParams.Solution.x, optimizedParams.Solution.y);
			auto s2 = second->Evaluate(optimizedParams.Solution.z, optimizedParams.Solution.w);
			auto optimizedDistance = mat::LengthSquared(s1 - s2);

			if (optimizedDistance < bestDistance)
			{
				bestGuess = optimizedParams.Solution;
				bestDistance = optimizedDistance;
			}
		}
		
		mat::NewtonSD newton(first, second);
		bestGuess = newton.Minimize(bestGuess, {}, 0.1).Solution;
		auto midpoint = 0.5f * (first->Evaluate(bestGuess.x, bestGuess.y) + second->Evaluate(bestGuess.z, bestGuess.w));

		return midpoint;
	}

	ICData Intersection::IntersectionCurve(ar::Entity firstObject, ar::Entity secondObject, float d)
	{
		ICData result;
		
		// =========== Config
		const size_t samples = 10;
		const double loopCloseEpsilon = 0.001;
		double precision = 1e-3;
		size_t iterations = 50;

		// =========== Algorithm
		bool selfIntersection = IsSelfIntersection(firstObject, secondObject);
		auto g1 = Parametric::Create(firstObject), g2 = Parametric::Create(secondObject);
		auto newton = mat::NewtonSD(g1, g2);

		auto startParameter = StartingParams(g1, g2, selfIntersection);
		if (selfIntersection) 
		{
			float uvDistance = ((startParameter.x - startParameter.z) * (startParameter.x - startParameter.z) +
				(startParameter.y - startParameter.w) * (startParameter.y - startParameter.w));
			if (uvDistance < 0.01) 
				return result;
		}

		mat::Vec3d p1 = g1->Evaluate(startParameter.x, startParameter.y);
		mat::Vec3d p2 = g2->Evaluate(startParameter.z, startParameter.w);

		mat::Vec4d start = startParameter;
		mat::Vec4d params = start;
		mat::Vec4d prevParams = params;

		mat::Vec3d PdU = g1->DerivativeU(params.x, params.y);
		mat::Vec3d PdV = g1->DerivativeV(params.x, params.y);
		mat::Vec3d QdU = g2->DerivativeU(params.z, params.w);
		mat::Vec3d QdV = g2->DerivativeV(params.z, params.w);

		mat::Vec3d prevP1 = g1->Evaluate(params.x, params.y);
		mat::Vec3d prevP2 = g2->Evaluate(params.z, params.w);
		mat::Vec3d prevMidpoint = (prevP1 + prevP2) * 0.5;

		mat::Vec3d startPoint = (g1->Evaluate(params.x, params.y) + g2->Evaluate(params.z, params.w)) * 0.5;
		bool success = false;

		p1 = g1->Evaluate(params.x, params.y);
		p2 = g2->Evaluate(params.z, params.w);
		if (mat::LengthSquared(p1 - p2) > precision) 
		{
			return result;
		}

		int i = 0;
		for (i = 0; i < iterations; ++i) 
		{
			// IMPORTANT: Keep startPoint FIXED for this Newton solve
			startPoint = (g1->Evaluate(params.x, params.y) + g2->Evaluate(params.z, params.w)) * 0.5;

			// Pass the fixed 3D startPoint to Newton
			auto newtonResult = newton.Minimize(params, startPoint, d);
			params = newtonResult.Solution;
			bool success = newtonResult.Converged;

			p1 = g1->Evaluate(params.x, params.y);
			p2 = g2->Evaluate(params.z, params.w);
			mat::Vec3d midpoint = (p1 + p2) * 0.5;

			// Retry with same startPoint but smaller steps
			for (int k = 0; k < 5 && (!success || mat::LengthSquared(midpoint - prevMidpoint) > precision); k++)
			{
				// Use same startPoint, but smaller d
				newtonResult = newton.Minimize(prevParams, startPoint, d * powf(0.5, k + 1));
				success = newtonResult.Converged;
				params = newtonResult.Solution;

				p1 = g1->Evaluate(params.x, params.y);
				p2 = g2->Evaluate(params.z, params.w);
				//midpoint = (p1 + p2) * 0.5;
			}

			if (!Clamp(g1, g2, params))
				break;
			
			if (result.Params.size() > 0 && mat::Length(midpoint - result.Points[0]) < loopCloseEpsilon && i > 5) 
			{
				result.Points.push_back(result.Points[0]);
				result.Params.push_back(result.Params[0]);
				return result;
			}

			if (!success || mat::Dot(midpoint - prevMidpoint, midpoint - prevMidpoint) > precision) 
			{
				prevParams = params;
				startPoint = midpoint;
				continue;
			}

			if (!result.Points.empty() && mat::LengthSquared(result.Points.back() - midpoint) < 1e-8)
			{
				prevParams = params;
				startPoint = midpoint;
				continue;
			}

			result.Points.push_back(midpoint);
			result.Params.push_back(params);

			prevParams = params;
			startPoint = midpoint;

			prevP1 = p1;
			prevP2 = p2;
			prevMidpoint = midpoint;
		}

		std::vector<mat::Vec3d> reverseCurve;
		std::vector<mat::Vec4d> reverseParameters;

		startPoint = (g1->Evaluate(startParameter.x, startParameter.y) +
			g2->Evaluate(startParameter.z, startParameter.w)) *
			0.5;
		params = startParameter;
		for (int i = 0; i < iterations; ++i) 
		{
			startPoint = (g1->Evaluate(params.x, params.y) + g2->Evaluate(params.z, params.w)) * 0.5;
			start = params;

			auto newtonResult = newton.Minimize(start, startPoint, -d);
			params = newtonResult.Solution;
			bool success = newtonResult.Converged;

			mat::Vec3d p1 = g1->Evaluate(params.x, params.y);
			mat::Vec3d p2 = g2->Evaluate(params.z, params.w);
			mat::Vec3d midpoint = (p1 + p2) * 0.5;

			for (int k = 0; k < 5 && (!success || mat::LengthSquared(midpoint - prevMidpoint) > precision); k++)
			{
				newtonResult = newton.Minimize(prevParams, startPoint, -d * powf(0.5, k + 1));
				success = newtonResult.Converged;
				params = newtonResult.Solution;

				p1 = g1->Evaluate(params.x, params.y);
				p2 = g2->Evaluate(params.z, params.w);
			}

			if (!success || mat::Dot(p1 - p2, p1 - p2) > precision) 
			{
				prevParams = params;
				startPoint = midpoint;
				continue;
			}

			reverseCurve.push_back(midpoint);
			reverseParameters.push_back(params);

			prevParams = params;
			startPoint = midpoint;
		}

		result.Points.insert(result.Points.begin(), reverseCurve.rbegin(), reverseCurve.rend());
		result.Params.insert(result.Params.begin(), reverseParameters.rbegin(), reverseParameters.rend());

		return result;
	}

	mat::Vec4d Intersection::StartingParams(Ref<mat::IParametricSurface> first, 
		Ref<mat::IParametricSurface> second, bool isSelfIntersecting)
	{
		const size_t samples = 10;
		auto params = GenerateUVs(samples, isSelfIntersecting);
		auto cg = mat::ConjugateGradientSD(first, second);

		double bestDistance = std::numeric_limits<double>::max();
		mat::Vec4d bestGuess;

		for (auto& pair : params)
		{
			auto optimizedParams = cg.Minimize({ pair.x, pair.y, pair.z, pair.w });
			auto s1 = first->Evaluate(optimizedParams.Solution.x, optimizedParams.Solution.y);
			auto s2 = second->Evaluate(optimizedParams.Solution.z, optimizedParams.Solution.w);
			auto optimizedDistance = mat::LengthSquared(s1 - s2);

			if (optimizedDistance < bestDistance)
			{
				bestGuess = optimizedParams.Solution;
				bestDistance = optimizedDistance;
			}
		}
		return bestGuess;
	}

	std::vector<mat::Vec4d> Intersection::GenerateUVs(size_t samples, bool selfIntersect)
	{
		auto max = static_cast<int>(samples);
		std::vector<mat::Vec4d> paramPairs;

		for (int u = 0; u <= max; u++)
			for (int v = 0; v <= max; v++)
				for (int s = 0; s <= max; s++)
					for (int t = 0; t <= max; t++)
					{
						auto normU = static_cast<double>(u) / (max),
							normV = static_cast<double>(v) / (max),
							normS = static_cast<double>(s) / (max),
							normT = static_cast<double>(t) / (max);
						if (selfIntersect)
						{
							double distX = normU - normS, distY = normV - normT;
							if ((distX * distX) + (distY * distY) < 0.01)
								continue;
						}
						paramPairs.emplace_back(normU, normV, normS, normT);
					}

		return paramPairs;
	}

	void Intersection::DrawDerivatives(ar::Entity object, size_t samples)
	{
		auto params = GenerateUVPairs(samples, true);
		auto first = Parametric::Create(object);
		//ar::DebugRenderer::Clear();
		for (auto& pair : params)
		{
			auto startd = first->Evaluate(pair.x, pair.y);
			auto start = ar::mat::Vec3(startd.x, startd.y, startd.z);
			auto normald = first->Normal(pair.x, pair.y);
			auto normal = ar::mat::Vec3(normald.x, normald.y, normald.z);

			ar::DebugRenderer::AddLine(start, start + 0.01f * normal);
		}
	}

	void Intersection::DrawEvaluations(ar::Entity object, size_t samples)
	{
		auto params = GenerateUVPairs(samples, true);
		auto first = Parametric::Create(object);
		//ar::DebugRenderer::Clear();
		for (auto& pair : params)
		{
			auto startd = first->Evaluate(pair.x, pair.y);
			auto start = ar::mat::Vec3(startd.x, startd.y, startd.z);
			ar::DebugRenderer::AddPoint(start, { 0.5, 0.5, 1.f });
		}
	}

	bool Intersection::Clamp(Ref<mat::IParametricSurface> first, Ref<mat::IParametricSurface> second, mat::Vec4d& params)
	{
		auto c1 = first->Clamp(params.x, params.y);
		auto c2 = second->Clamp(params.z, params.w);
		return c1 && c2;
	}

	std::vector<ar::mat::Vec4> Intersection::GenerateUVPairs(size_t samples, bool selfIntersect)
	{
		auto max = static_cast<int>(samples);
		std::vector<mat::Vec4> paramPairs;

		for (int u = 0; u <= max; u++)
			for (int v = 0; v <= max; v++)
				for (int s = 0; s <= max; s++)
					for (int t = 0; t <= max; t++)
					{
						auto normU = static_cast<float>(u) / (max),
							normV = static_cast<float>(v) / (max),
							normS = static_cast<float>(s) / (max),
							normT = static_cast<float>(t) / (max);
						if (selfIntersect)
						{
							float distX = normU - normS, distY = normV - normT;
							if ((distX * distX) + (distY * distY) < 0.01)
								continue;
						}
						paramPairs.emplace_back(normU, normV, normS, normT);
					}
						
		return paramPairs;
	}

	bool Intersection::IsSelfIntersection(ar::Entity first, ar::Entity second)
	{
		return first.GetID() == second.GetID();
	}
}
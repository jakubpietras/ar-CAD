#include "arpch.h"
#include "Intersection.h"
#include <core/Scene/Components.h>
#include "core/Utils/GeneralUtils.h"
#include <algorithm>
#include <ranges>
#include <limits>
#include "core/Utils/SurfaceUtils.h"

namespace ar
{
	ar::mat::Vec3 Intersection::FindStartingPointDebug(ar::Entity firstObject, ar::Entity secondObject)
	{
		const size_t samples = 10;
		
		//return Evaluate(firstObject, 1. / 3, 1. / 3);

		auto params = CalculateStartingParams(firstObject, secondObject, samples);
		return Evaluate(firstObject, params.x, params.y);
	}

	ar::mat::Vec4 Intersection::CalculateStartingParams(ar::Entity firstObject, ar::Entity secondObject, size_t samples)
	{
		auto params = GenerateUVPairs(samples);
		
		float distance = std::numeric_limits<float>::max();
		mat::Vec4 bestGuess;

		for (auto& pair : params)
		{
			auto optimizedParams = CGSquaredDistance(firstObject, secondObject, pair);
			auto optimizedDistance = ComputeRealDistance(firstObject, secondObject, optimizedParams);
			if (optimizedDistance < distance)
			{
				bestGuess = optimizedParams;
				distance = optimizedDistance;
			}
		}
		return bestGuess;
	}

	std::vector<ar::mat::Vec4> Intersection::GenerateUVPairs(size_t samples)
	{
		auto max = static_cast<int>(samples);
		std::vector<mat::Vec4> paramPairs;

		for (int u = 0; u <= max; u++)
			for (int v = 0; v <= max; v++)
				for (int s = 0; s <= max; s++)
					for (int t = 0; t <= max; t++)
						paramPairs.emplace_back(
							static_cast<float>(u) / (max),
							static_cast<float>(v) / (max),
							static_cast<float>(s) / (max),
							static_cast<float>(t) / (max)
						);
		return paramPairs;
	}

	float Intersection::ComputeRealDistance(ar::Entity firstObject, ar::Entity secondObject, mat::Vec4 params)
	{
		auto firstPoint = Evaluate(firstObject, params.x, params.y);
		auto secondPoint = Evaluate(secondObject, params.z, params.w);
		return mat::Length(firstPoint - secondPoint);
	}

	ar::mat::Vec4 Intersection::CGSquaredDistance(ar::Entity firstObject, ar::Entity secondObject, mat::Vec4 initial)
	{
		// Conjugate Gradient Minimization for Squared Distance function
		const int maxIterations = 50;
		const float eps = 1e-6;

		mat::Vec4 
			prevGradient = SquaredDistanceGradient(firstObject, secondObject, initial),
			currGradient = prevGradient,
			prevDir = -currGradient,
			currDir = prevDir,
			prevParams = initial,
			currParams = prevParams;

		float gradNormSq = mat::LengthSquared(currGradient);
		if (!std::isfinite(gradNormSq) || gradNormSq == 0.0f)
			return currParams;

		for (int iter = 0; iter < maxIterations; ++iter)
		{
			if (gradNormSq < eps) 
				break;

			float alpha = LineSearchSquaredDistance(firstObject, secondObject, prevParams, prevDir);
			if (alpha < 1e-10f) 
			{
				// Line search failed, try steepest descent
				prevDir = -currGradient;
				alpha = LineSearchSquaredDistance(firstObject, secondObject, prevParams, prevDir);
				if (alpha < 1e-10f) break; // Give up
			}
			currParams = prevParams + alpha * prevDir;	
			ClampWrapObjects(firstObject, secondObject, currParams);
			
			currGradient = SquaredDistanceGradient(firstObject, secondObject, currParams);
			float gNewNormSq = mat::LengthSquared(currGradient);
			if (!std::isfinite(gNewNormSq)) 
				break;

			auto beta = PolakRibiere(prevGradient, currGradient);
			currDir = -currGradient + beta * prevDir;

			prevDir = currDir;
			prevGradient = currGradient;
			prevParams = currParams;
		}

		return currParams;	
	}

	float Intersection::LineSearchSquaredDistance(ar::Entity firstObject, ar::Entity secondObject, ar::mat::Vec4 params, ar::mat::Vec4 direction)
	{
		// Backtracking line search
		const float c = 1e-4;		// Armijo parameter
		const float tau = 0.5f;		// shrink factor
		float alpha = 1.f;
		auto point = params;

		auto grad = SquaredDistanceGradient(firstObject, secondObject, params);
		float slope = mat::Dot(grad, direction);

		if (!std::isfinite(slope) || slope >= 0.0f) 
		{
			direction = -grad;
			slope = mat::Dot(grad, direction);
			if (slope >= 0.0f || !std::isfinite(slope)) 
			{
				return 1e-6f;
			}
		}

		float f0 = SquaredDistanceValue(firstObject, secondObject, params);
		for (int iter = 0; iter < 50; ++iter) 
		{
			ar::mat::Vec4 candidate = params + alpha * direction;
			ClampWrapObjects(firstObject, secondObject, candidate);
			float fCandidate = SquaredDistanceValue(firstObject, secondObject, candidate);

			if (fCandidate <= f0 + c * alpha * slope) 
			{
				return alpha;
			}
			alpha *= tau;
			if (alpha < 1e-12f) 
				break;
		}
		
		return alpha;
	}

	float Intersection::SquaredDistanceValue(ar::Entity firstObject, ar::Entity secondObject, ar::mat::Vec4 params)
	{
		auto s1 = Evaluate(firstObject, params.x, params.y);
		auto s2 = Evaluate(secondObject, params.z, params.w);
		return mat::LengthSquared(s1 - s2);
	}

	void Intersection::ClampWrapUV(ar::Entity obj, float& u, float& v)
	{
		if (obj.HasComponent<ar::TorusComponent>())
		{
			if (u > 1.0f) u = u - std::floor(u);
			else if (u < 0.0f) u = u - std::floor(u);

			if (v > 1.0f) v = v - std::floor(v);
			else if (v < 0.0f) v = v - std::floor(v);
		}
		else
		{
			u = std::clamp(u, 0.0f, 1.0f);
			v = std::clamp(v, 0.0f, 1.0f);
		}
	}

	void Intersection::ClampWrapObjects(ar::Entity o1, ar::Entity o2, ar::mat::Vec4& p)
	{
		ClampWrapUV(o1, p.x, p.y);
		ClampWrapUV(o2, p.z, p.w);
	}

	float Intersection::PolakRibiere(mat::Vec4 prevGrad, mat::Vec4 currGrad)
	{
		mat::Vec4 y = currGrad - prevGrad;
		float denom = mat::LengthSquared(prevGrad);
		float beta = 0.0f;
		if (denom > 0.0f) 
		{
			beta = mat::Dot(currGrad, y) / denom;
			if (beta < 0.0f) beta = 0.0f; // reset if negative (PR+)
		}
		return beta;
	}

	ar::SegmentInfo Intersection::MapMultipatch(ar::Entity patch, float u, float v)
	{
		auto& desc = patch.GetComponent<ar::SurfaceComponent>().Description;

		SegmentInfo info;
		info.scaling = { static_cast<float>(desc.Segments.u), static_cast<float>(desc.Segments.v) };

		if (desc.Segments.u == 1 && desc.Segments.v == 1) 
		{
			info.segment = { 0, 0 };
			info.localParams = { std::clamp(u, 0.0f, 1.0f), std::clamp(v, 0.0f, 1.0f) };
			info.scaling = { 1.0f, 1.0f };
			return info;
		}

		u = std::clamp(u, 0.0f, 1.0f);
		v = std::clamp(v, 0.0f, 1.0f);

		const float segmentWidth = 1.0f / desc.Segments.u;
		const float segmentHeight = 1.0f / desc.Segments.v;

		// Which segment
		int segmentU = (u >= 1.0f) ? desc.Segments.u - 1 :
			static_cast<int>(std::floor(u / segmentWidth));
		int segmentV = (v >= 1.0f) ? desc.Segments.v - 1 :
			static_cast<int>(std::floor(v / segmentHeight));

		segmentU = std::clamp(segmentU, 0, static_cast<int>(desc.Segments.u - 1));
		segmentV = std::clamp(segmentV, 0, static_cast<int>(desc.Segments.v - 1));

		info.segment = { static_cast<float>(segmentU), static_cast<float>(segmentV) };

		// Local parameters
		const float segmentStartU = segmentU * segmentWidth;
		const float segmentStartV = segmentV * segmentHeight;

		// Boundary
		float localU, localV;

		if (u >= 1.0f || (segmentU == desc.Segments.u - 1 && u >= segmentStartU + segmentWidth)) {
			localU = 1.0f;
		}
		else {
			localU = (u - segmentStartU) / segmentWidth;
		}

		if (v >= 1.0f || (segmentV == desc.Segments.v - 1 && v >= segmentStartV + segmentHeight)) {
			localV = 1.0f;
		}
		else {
			localV = (v - segmentStartV) / segmentHeight;
		}

		info.localParams = { std::clamp(localU, 0.0f, 1.0f), std::clamp(localV, 0.0f, 1.0f) };

		return info;
	}

	ar::mat::Vec4 Intersection::NewtonMinimization(ar::Entity firstObject, ar::Entity secondObject, mat::Vec4 initial)
	{
		throw new std::runtime_error("Not implemented");
	}

	ar::mat::Vec3 Intersection::DerivativeU(ar::Entity object, float u, float v)
	{
		if (object.HasComponent<ar::TorusComponent>())
		{
			auto desc = object.GetComponent<ar::TorusComponent>().Description;
			auto model = object.GetComponent<ar::TransformComponent>().ModelMatrix;
			auto derivative = model * mat::Vec4(ar::mat::DerivativeUTorus(desc.SmallRadius, desc.LargeRadius, u, v), 0.f);

			return mat::Vec3(derivative);
		}
		if (object.HasComponent<ar::SurfaceComponent>())
		{
			auto desc = object.GetComponent<ar::SurfaceComponent>().Description;
			auto info = MapMultipatch(object, u, v);
			auto points = ar::SurfaceUtils::GetSegmentPoints(object, info.segment);

			if (desc.Type == SurfaceType::RECTANGLEC2 || desc.Type == SurfaceType::CYLINDERC2)
			{
				info.localParams.x = 1. / 3 + (2. / 3 - 1. / 3) * info.localParams.x;
				info.localParams.y = 1. / 3 + (2. / 3 - 1. / 3) * info.localParams.y;
				return
					1./3 *
					info.scaling.x *
					ar::mat::DerivativeUBezierPatch(GeneralUtils::GetPos(points), info.localParams.x, info.localParams.y);
			}
			else
				return 
					info.scaling.x *
					ar::mat::DerivativeUBezierPatch(GeneralUtils::GetPos(points), info.localParams.x, info.localParams.y);
		}
		return { 0.f, 0.f, 0.f };
	}

	ar::mat::Vec3 Intersection::DerivativeV(ar::Entity object, float u, float v)
	{
		if (object.HasComponent<ar::TorusComponent>())
		{
			auto desc = object.GetComponent<ar::TorusComponent>().Description;
			auto model = object.GetComponent<ar::TransformComponent>().ModelMatrix;
			auto derivative = model * mat::Vec4(ar::mat::DerivativeVTorus(desc.SmallRadius, desc.LargeRadius, u, v), 0.f);

			return mat::Vec3(derivative);
		}
		if (object.HasComponent<ar::SurfaceComponent>())
		{
			auto desc = object.GetComponent<ar::SurfaceComponent>().Description;
			auto info = MapMultipatch(object, u, v);
			auto points = ar::SurfaceUtils::GetSegmentPoints(object, info.segment);

			if (desc.Type == SurfaceType::RECTANGLEC2 || desc.Type == SurfaceType::CYLINDERC2)
			{
				info.localParams.x = 1. / 3 + (2. / 3 - 1. / 3) * info.localParams.x;
				info.localParams.y = 1. / 3 + (2. / 3 - 1. / 3) * info.localParams.y;
				return
					1. / 3 *
					info.scaling.y *
					ar::mat::DerivativeVBezierPatch(GeneralUtils::GetPos(points), info.localParams.x, info.localParams.y);
			}
			else
				return
				info.scaling.y *
				ar::mat::DerivativeVBezierPatch(GeneralUtils::GetPos(points), info.localParams.x, info.localParams.y);
		}
		return { 0.f, 0.f, 0.f };
	}

	ar::mat::Vec4 Intersection::SquaredDistanceGradient(ar::Entity firstObject, ar::Entity secondObject, mat::Vec4 params)
	{
		auto u = params.x, v = params.y, s = params.z, t = params.w;
		auto difference = Evaluate(firstObject, u, v) - Evaluate(secondObject, s, t);
		
		auto dS1du = DerivativeU(firstObject, u, v), dS1dv = DerivativeV(firstObject, u, v);
		auto dS2ds = DerivativeU(secondObject, s, t), dS2dt = DerivativeV(secondObject, s, t);

		return {
			2 * mat::Dot(difference, dS1du),
			2 * mat::Dot(difference, dS1dv),
			- 2 * mat::Dot(difference, dS2ds),
			- 2 * mat::Dot(difference, dS2dt)
		};
	}

	ar::mat::Vec3 Intersection::Evaluate(ar::Entity object, float u, float v)
	{
		if (object.HasComponent<ar::TorusComponent>())
		{
			auto desc = object.GetComponent<ar::TorusComponent>().Description;
			auto model = object.GetComponent<ar::TransformComponent>().ModelMatrix;
			auto point = model * mat::Vec4(ar::mat::EvaluateTorus(desc.SmallRadius, desc.LargeRadius, u, v), 1.f);
			return mat::Vec3(point);
		}
		if (object.HasComponent<ar::SurfaceComponent>())
		{
			auto desc = object.GetComponent<ar::SurfaceComponent>().Description;
			auto info = MapMultipatch(object, u, v);
			auto segmentPoints = ar::SurfaceUtils::GetSegmentPoints(object, info.segment);
			if (desc.Type == SurfaceType::RECTANGLEC2 || desc.Type == SurfaceType::CYLINDERC2)
			{
				info.localParams.x = 1. / 3 + (2. / 3 - 1. / 3) * info.localParams.x;
				info.localParams.y = 1. / 3 + (2. / 3 - 1. / 3) * info.localParams.y;
			}
			return ar::mat::EvaluateBezierPatch(GeneralUtils::GetPos(segmentPoints), info.localParams.x, info.localParams.y);
		}
		return { 0.f, 0.f, 0.f };
	}

	void Intersection::TestGradient(ar::Entity obj1, ar::Entity obj2)
	{
	}

}
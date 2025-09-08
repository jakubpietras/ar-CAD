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
	ar::mat::Vec3 Intersection::FindStartingPoint(ar::Entity firstObject, ar::Entity secondObject)
	{
		const size_t samples = 10;
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

			currGradient = SquaredDistanceGradient(firstObject, secondObject, currParams);
			// beta
			auto beta = PolakRibiere(prevGradient, currGradient);
			
			// conjugate direction
			currDir = -currGradient + beta * prevDir;

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
			
			float gNewNormSq = mat::LengthSquared(currGradient);
			if (!std::isfinite(gNewNormSq)) 
				break;

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
		for (int iter = 0; iter < 20; ++iter) 
		{
			ar::mat::Vec4 candidate = params + alpha * direction;
			ClampWrapObjects(firstObject, secondObject, candidate);
			float fCandidate = SquaredDistanceValue(firstObject, secondObject, candidate);

			if (fCandidate <= f0 + c * alpha * slope) 
			{
				return alpha;
			}
			alpha *= tau;
			if (alpha < 1e-8f) 
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

	bool Intersection::ClampWrapUV(ar::Entity obj, float& u, float& v)
	{
		bool success = true;

		auto wrapValue = [](float& val, bool isWrapped) -> bool 
			{
			if (val > 1.0f && isWrapped) 
			{
				int n = (int)val;
				val -= n;
			}
			else if (val < 0.0f && isWrapped) 
			{
				int n = (int)val;
				val = val - n;  // Makes positive
			}
			else if (val < 0.0f || val > 1.0f) 
			{
				val = std::clamp(val, 0.0f, 1.0f);
				return false;  // Clamping occurred
			}
			return true;
			};

		// Determine wrapping behavior per surface type
		bool uWrapped = false, vWrapped = false;

		if (obj.HasComponent<ar::TorusComponent>()) 
		{
			uWrapped = vWrapped = true;
		}
		else if (obj.HasComponent<ar::SurfaceComponent>()) 
		{
			auto& desc = obj.GetComponent<ar::SurfaceComponent>().Description;
			if (desc.Type == SurfaceType::CYLINDERC2 || desc.Type == SurfaceType::CYLINDERC0) 
			{
				uWrapped = true;  // Only U wrapped for cylinders
				vWrapped = false;
			}
		}

		success &= wrapValue(u, uWrapped);
		success &= wrapValue(v, vWrapped);

		return success;
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
			info.localParams = { u, v };
			info.scaling = { 1.0f, 1.0f };
			return info;
		}

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

		info.localParams = { localU, localV };

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
			auto& surf = object.GetComponent<ar::SurfaceComponent>();
			auto desc = surf.Description;
			auto info = MapMultipatch(object, u, v);

			if (desc.Type == SurfaceType::RECTANGLEC2 || desc.Type == SurfaceType::CYLINDERC2)
			{
				auto bezier = ar::SurfaceUtils::GetSegmentPointsBezier(*surf.AuxPoints, desc, info.segment);
				return info.scaling.x *
					ar::mat::DerivativeUBezierPatch(bezier, info.localParams.x, info.localParams.y);
			}
			else
			{
				auto points = ar::SurfaceUtils::GetSegmentPoints(object, info.segment);
				return info.scaling.x *
					ar::mat::DerivativeUBezierPatch(GeneralUtils::GetPos(points),
						info.localParams.x, info.localParams.y);
			}
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
			auto& surf = object.GetComponent<ar::SurfaceComponent>();
			auto desc = surf.Description;
			auto info = MapMultipatch(object, u, v);
			
			if (desc.Type == SurfaceType::RECTANGLEC2 || desc.Type == SurfaceType::CYLINDERC2)
			{
				auto bezier = ar::SurfaceUtils::GetSegmentPointsBezier(*surf.AuxPoints, desc, info.segment);
				return info.scaling.y *
					ar::mat::DerivativeVBezierPatch(bezier, info.localParams.x, info.localParams.y);
			}
			else
			{
				auto points = ar::SurfaceUtils::GetSegmentPoints(object, info.segment);
				return info.scaling.y *
					ar::mat::DerivativeVBezierPatch(GeneralUtils::GetPos(points),
						info.localParams.x, info.localParams.y);
			}
		}
		return { 0.f, 0.f, 0.f };
	}

	ar::mat::Vec4 Intersection::SquaredDistanceGradient(ar::Entity firstObject, ar::Entity secondObject, mat::Vec4 params)
	{
		auto u = params.x, v = params.y, s = params.z, t = params.w;
		auto difference = Evaluate(firstObject, u, v) - Evaluate(secondObject, s, t);
		
		//ClampWrapObjects(firstObject, secondObject, params);
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
			auto& surf = object.GetComponent<ar::SurfaceComponent>();
			auto desc = surf.Description;
			auto info = MapMultipatch(object, u, v);

			if (desc.Type == SurfaceType::RECTANGLEC2 || desc.Type == SurfaceType::CYLINDERC2)
			{
				auto bezier = ar::SurfaceUtils::GetSegmentPointsBezier(*surf.AuxPoints, desc, info.segment);
				return ar::mat::EvaluateBezierPatch(bezier, info.localParams.x, info.localParams.y);
			}
			
			auto points = ar::SurfaceUtils::GetSegmentPoints(object, info.segment);
			return ar::mat::EvaluateBezierPatch(GeneralUtils::GetPos(points), info.localParams.x, info.localParams.y);
		}
		return { 0.f, 0.f, 0.f };
	}
}
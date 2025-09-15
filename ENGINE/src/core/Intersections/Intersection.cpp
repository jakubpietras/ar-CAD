#include "arpch.h"
#include "Intersection.h"
#include <core/Scene/Components.h>
#include "core/Utils/GeneralUtils.h"
#include <algorithm>
#include <ranges>
#include <limits>
#include "core/Utils/SurfaceUtils.h"
#include "core/Scene/DebugRenderer.h"

namespace ar
{
	ar::mat::Vec3 Intersection::FindStartingPoint(ar::Entity firstObject, ar::Entity secondObject)
	{
		const size_t samples = 10;
		auto params = CalculateStartingParams(firstObject, secondObject, samples);
		ar::mat::Vec4 newtonized;
		auto result = NewtonMinimization(newtonized, firstObject, secondObject, params, 0.02);

		auto midpointUnoptimized = 0.5f * (Evaluate(firstObject, params.x, params.y) + Evaluate(secondObject, params.z, params.w));
		auto midpointNewtonized = 0.5f * (Evaluate(firstObject, newtonized.x, newtonized.y) + Evaluate(secondObject, newtonized.z, newtonized.w));

		return midpointNewtonized;
	}

	std::pair<std::vector<ar::mat::Vec3>, std::vector<ar::mat::Vec4>> Intersection::TraceIntersectionCurve(ar::Entity firstObject, ar::Entity secondObject, float step)
	{
		const float precision = 1e-4;
		const size_t iterations = 100;
		float loopEps = std::max(1.5f * step, 1e-3f);
		float d = step;

		std::vector<mat::Vec3> curvePoints;		// 3D points in the scene that form the curve
		std::vector<mat::Vec4> curveParams;		// 2D parameters of both curves (UV, ST)

		ar::mat::Vec4 params, prevParams, optimizedParams;
		ar::mat::Vec3 point, prevPoint;

		// STARTING PARAMETERS
		auto startParams = CalculateStartingParams(firstObject, secondObject, 10);
		
		// Self-intersection
		if (IsSelfIntersection(firstObject, secondObject))
		{
			auto dx = startParams.x - startParams.z;
			auto dy = startParams.y - startParams.w;
			if (dx * dx + dy * dy < 0.01f)
				return { curvePoints, curveParams };
		}
		// Initial guess is not a valid intersection
		auto p1 = Evaluate(firstObject, startParams.x, startParams.y);
		auto p2 = Evaluate(secondObject, startParams.z, startParams.w);
		mat::Vec3 startPoint = 0.5f * (p1 + p2);
		/*if (mat::LengthSquared(p1 - p2) > 1e-2) {
			return { curvePoints, curveParams };
		}*/
		
		params = prevParams = startParams;
		point = prevPoint = startPoint;

		for (size_t i = 0; i < iterations; i++)
		{
			// Optimize the parameters with Newton method
			bool success = NewtonMinimization(optimizedParams, firstObject, secondObject, params, d);

			// After moving d units along the curve, compute a new point
			auto midP1 = Evaluate(firstObject, optimizedParams.x, optimizedParams.y);
			auto midP2 = Evaluate(secondObject, optimizedParams.z, optimizedParams.w);
			auto midpoint = 0.5f * (midP1 + midP2);
			auto midpointDiff = midpoint - prevPoint;

			// Retry Newton 5 times if attempt was unsuccessful or it overshot
			params = optimizedParams;
			for (size_t k = 0; k < 5 && (!success || mat::Dot(midpointDiff, midpointDiff) > precision); k++)
			{
				auto newStep = d / (powf(2, k + 1));
				success = NewtonMinimization(params, firstObject, secondObject, optimizedParams, newStep);
				
				midP1 = Evaluate(firstObject, params.x, params.y);
				midP2 = Evaluate(secondObject, params.z, params.w);
				midpoint = 0.5f * (midP1 + midP2);
				midpointDiff = midpoint - prevPoint;
			}

			// -----  At this stage, 'params' is the established (u1, v1, s1, t1)  -------

			// HANDLE STEPPING OUTSIDE THE DOMAIN
			if (!ClampWrapObjects(firstObject, secondObject, params))
			{
				break;
			}
			// HANDLE LOOPING
			if (!curveParams.empty() && (mat::Length(midpoint - curvePoints[0]) < loopEps) && i > 5)
			{
				curvePoints.push_back(curvePoints[0]);
				curveParams.push_back(curveParams[0]);
				return { curvePoints, curveParams };
			}
			if (!success || mat::LengthSquared(midP1 - midP2) > precision) {
				prevParams = params;
				continue;
			}
			// HANDLE DUPLICATE POINTS
			if (!curvePoints.empty() && mat::LengthSquared(curvePoints.back() - midpoint) < 1e-8f)
			{
				prevParams = params;
				continue;
			}

			// No edge cases detected - accept this point
			curvePoints.push_back(midpoint);
			curveParams.push_back(params);

			prevParams = params;
			prevPoint = midpoint;
		}

		std::vector<mat::Vec3> reverseCurvePoints;
		std::vector<mat::Vec4> reverseCurveParams;

		params = optimizedParams = startParams;
		prevPoint = startPoint;
		for (size_t i = 0; i < iterations; i++)
		{
			// Negative step to go in the other direction
			auto success = NewtonMinimization(optimizedParams, firstObject, secondObject, params, -d);
			auto midP1 = Evaluate(firstObject, optimizedParams.x, optimizedParams.y);
			auto midP2 = Evaluate(secondObject, optimizedParams.z, optimizedParams.w);
			auto midpoint = 0.5f * (midP1 + midP2);
			auto midpointDiff = midpoint - prevPoint;

			params = optimizedParams;
			for (size_t k = 0; k < 5 && (!success || mat::Dot(midpointDiff, midpointDiff) > precision); k++)
			{
				auto newStep = -d / (powf(2, k + 1));
				success = NewtonMinimization(params, firstObject, secondObject, optimizedParams, newStep);
				
				midP1 = Evaluate(firstObject, params.x, params.y);
				midP2 = Evaluate(secondObject, params.z, params.w);
				midpoint = 0.5f * (midP1 + midP2);
				midpointDiff = midpoint - prevPoint;
			}

			if (!success || mat::LengthSquared(midP1 - midP2) > precision) 
			{
				prevParams = params;
				continue;
			}

			reverseCurvePoints.push_back(midpoint);
			reverseCurveParams.push_back(params);

			prevParams = params;
			prevPoint = midpoint;
		}

		// Stitch forward and reverse curve
		curvePoints.insert(curvePoints.begin(), reverseCurvePoints.rbegin(), reverseCurvePoints.rend());
		curveParams.insert(curveParams.begin(), reverseCurveParams.rbegin(), reverseCurveParams.rend());

		return { curvePoints, curveParams };
	}

	void Intersection::DrawDerivatives(ar::Entity object, size_t samples)
	{
		auto params = GenerateUVPairs(samples, true);
		ar::DebugRenderer::Clear();
		for (auto& pair : params)
		{
			auto start = Evaluate(object, pair.x, pair.y);
			auto derivativeU = DerivativeU(object, pair.x, pair.y);
			auto derivativeV = DerivativeV(object, pair.x, pair.y);
			ar::DebugRenderer::AddLine(start, start + 0.01f * mat::Cross(derivativeV, derivativeU));
		}
	}

	void Intersection::DrawEvaluations(ar::Entity object, size_t samples)
	{
		auto params = GenerateUVPairs(samples, true);
		ar::DebugRenderer::Clear();
		for (auto& pair : params)
		{
			auto start = Evaluate(object, pair.x, pair.y);
			ar::DebugRenderer::AddPoint(start, { 0.5, 0.5, 1.f });
		}
	}

	ar::mat::Vec4 Intersection::CalculateStartingParams(ar::Entity firstObject, ar::Entity secondObject, size_t samples)
	{
		bool selfIntersection = IsSelfIntersection(firstObject, secondObject);
		auto params = GenerateUVPairs(samples, selfIntersection);
		
		float bestDistance = std::numeric_limits<float>::max();
		mat::Vec4 bestGuess;

		for (auto& pair : params)
		{
			auto optimizedParams = CGSquaredDistance(firstObject, secondObject, pair);
			auto optimizedDistance = SquaredDistanceValue(firstObject, secondObject, optimizedParams);
			
			if (optimizedDistance < bestDistance)
			{
				if (selfIntersection)
				{
					float distX = optimizedParams.x - optimizedParams.z;
					float distY = optimizedParams.y - optimizedParams.w;
					if ((distX * distX) + (distY * distY) < 0.01)
						continue;
				}
				bestGuess = optimizedParams;
				bestDistance = optimizedDistance;
			}
		}
		return bestGuess;
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

		// Initialization
		mat::Vec4 currParams = initial;
		mat::Vec4 currGradient = SquaredDistanceGradient(firstObject, secondObject, currParams);
		float gradNormSq = mat::LengthSquared(currGradient);
		if (!std::isfinite(gradNormSq) || gradNormSq < eps)
			return currParams;
		mat::Vec4 currDir = -currGradient;

		// 0th iteration
		float alpha = LineSearchSquaredDistance(firstObject, secondObject, currParams, currDir);
		if (alpha < 1e-10f)
		{
			// Line search completely fails
			return currParams;
		}
		currParams = currParams + alpha * currDir;
		ClampWrapObjects(firstObject, secondObject, currParams);
		mat::Vec4 prevGradient = currGradient;
		mat::Vec4 prevDir = currDir;

		for (int iter = 0; iter < maxIterations; ++iter)
		{
			currGradient = SquaredDistanceGradient(firstObject, secondObject, currParams);
			gradNormSq = mat::LengthSquared(currGradient);
			if (!std::isfinite(gradNormSq) || gradNormSq < eps)
				break;

			// beta
			float beta = PolakRibiere(prevGradient, currGradient);
			if (iter % 20 == 0) 
			{
				beta = 0.0f;  // fallback to steepest descent
			}
			
			// conjugate direction update
			currDir = -currGradient + beta * prevDir;

			alpha = LineSearchSquaredDistance(firstObject, secondObject, currParams, currDir);
			if (alpha < 1e-10f) 
			{
				// Line search failed, try steepest descent
				currDir = -currGradient;
				alpha = LineSearchSquaredDistance(firstObject, secondObject, currParams, currDir);
				if (alpha < 1e-10f) 
					break; // Give up
			}

			// iteration update
			currParams = currParams + alpha * currDir;	
			ClampWrapObjects(firstObject, secondObject, currParams);
			prevDir = currDir;
			prevGradient = currGradient;
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
		auto p1 = Evaluate(firstObject, params.x, params.y);
		auto p2 = Evaluate(secondObject, params.z, params.w);
		
		float phi0 = mat::Length(p1 - p2);
		float slope = mat::Dot(grad, direction);

		if (slope >= 0.0f) 
		{
			return 1e-6f;
		}

		for (int iter = 0; iter < 20; ++iter) 
		{
			ar::mat::Vec4 candidate = params + alpha * direction;
			ClampWrapObjects(firstObject, secondObject, candidate);
			
			p1 = Evaluate(firstObject, candidate.x, candidate.y);
			p2 = Evaluate(secondObject, candidate.z, candidate.w);
			float fCandidate = mat::Length(p1 - p2);

			if (fCandidate <= phi0 + c * alpha * slope)
				return alpha;
			
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

		// TRUE if wrapped
		// FALSE if clamped
		auto wrapValue = [](float& val, bool isWrapped) -> bool 
			{
			if (val > 1.0f && isWrapped) 
			{
				int n = (int)val;
				val -= n;
			}
			else if (val < 0.0f && isWrapped)
			{
				val = val - std::floor(val);
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
				uWrapped = true;
				vWrapped = false;
			}
		}

		success &= wrapValue(u, uWrapped);
		success &= wrapValue(v, vWrapped);

		return success;
	}

	bool Intersection::ClampWrapObjects(ar::Entity o1, ar::Entity o2, ar::mat::Vec4& p)
	{
		auto cw1 = ClampWrapUV(o1, p.x, p.y);
		auto cw2 = ClampWrapUV(o2, p.z, p.w);
		return cw1 && cw2;
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

	bool Intersection::IsSelfIntersection(ar::Entity first, ar::Entity second)
	{
		return first.GetID() == second.GetID();
	}

	bool Intersection::NewtonMinimization(ar::mat::Vec4& result, ar::Entity firstObject, ar::Entity secondObject, mat::Vec4 initial, double distance)
	{
		const size_t iterations = 20;	// How many times the method can refine current step
		const float dampingFactor = 0.5f;
		const float precision = 1e-8;
		bool repeat = false;
		
		// Evaluate point at first guess (u0, v0, s0, t0)
		auto startPoint = 0.5f * (Evaluate(firstObject, initial.x, initial.y) + Evaluate(secondObject, initial.z, initial.w));
		
		// Compute versors of both surfaces at the start point (u0, v0, s0, t0)
		auto dS1du = DerivativeU(firstObject, initial.x, initial.y), dS1dv = DerivativeV(firstObject, initial.x, initial.y);
		auto dS2du = DerivativeU(secondObject, initial.z, initial.w), dS2dv = DerivativeV(secondObject, initial.z, initial.w);
		auto normalS1 = mat::Normalize(mat::Cross(dS1du, dS1dv));
		auto normalS2 = mat::Normalize(mat::Cross(dS2du, dS2dv));

		// Compute tangent (HANDLE PARALLEL NORMALS)
		auto tangent = mat::Normalize(mat::Cross(normalS1, normalS2));
		if (!isfinite(tangent.x) || !isfinite(tangent.y) || !isfinite(tangent.z) || mat::Length(tangent) < 1e-6) 
		{
			// (Nearly) parallel normals produce a (nearly) zero cross product
			tangent = mat::Normalize(dS1du);
			repeat = true; // if no convergence, try again with dS1dv later
		}

		ar::mat::Vec4	params = initial,			// x_{k+1}
						prevParams = initial;		// x_k

		for (size_t i = 0; i < iterations; i++)
		{
			auto functionValue = IntersectionFuncValue(firstObject, secondObject, params, tangent, startPoint, distance);
			auto jacobian = IntersectionFuncJacobian(firstObject, secondObject, params, tangent);
			auto delta = mat::SolveLinear(jacobian, functionValue);
			
			prevParams = params;
			params -= delta * dampingFactor;

			// HANDLE WALKING OUTSIDE THE DOMAIN:
			if (!ClampWrapObjects(firstObject, secondObject, params))
			{
				result = prevParams;
				return false;
			}
			// HANDLE NUMERICAL ERRORS
			if (!isfinite(params.x) || !isfinite(params.y) || !isfinite(params.z) || !isfinite(params.w)) {
				result = prevParams;
				return false;
			}

			// CONVERGENCE TEST (the change needed to apply to x_k is so small that we don't need to
			// keep going anymore. This is THE ONLY SUCCESS ROUTE, all other are fails
			if (mat::Dot(delta * dampingFactor, delta * dampingFactor) < precision)
			{
				result = params;
				return true;
			}

			prevParams = params;
			// Repeat with different tangent if normal calculation failed before the loop
			if (repeat && i == iterations - 1) 
			{
				tangent = mat::Normalize(dS1dv);
				repeat = false;
				i = 0;
			}
		}

		// NO CONVERGENCE happened, returning current result and reporting failure
		result = params;
		return false;
	}

	ar::mat::Mat4 Intersection::IntersectionFuncJacobian(ar::Entity firstObject, ar::Entity secondObject, mat::Vec4 params, ar::mat::Vec3 tangent)
	{
		auto p = Evaluate(firstObject, params.x, params.y);
		auto q = Evaluate(secondObject, params.z, params.w);

		auto dPdu = DerivativeU(firstObject, params.x, params.y);
		auto dPdv = DerivativeV(firstObject, params.x, params.y);
		auto dQdu = DerivativeU(secondObject, params.z, params.w);
		auto dQdv = DerivativeV(secondObject, params.z, params.w);

		mat::Mat4 j;
		j(0, 0) = dPdu.x;
		j(1, 0) = dPdu.y;
		j(2, 0) = dPdu.z;
		j(3, 0) = 0.5f * mat::Dot(dPdu, tangent);

		j(0, 1) = dPdv.x;
		j(1, 1) = dPdv.y;
		j(2, 1) = dPdv.z;
		j(3, 1) = 0.5f * mat::Dot(dPdv, tangent);

		j(0, 2) = -dQdu.x;
		j(1, 2) = -dQdu.y;
		j(2, 2) = -dQdu.z;
		j(3, 2) = 0.5f * mat::Dot(dQdu, tangent);

		j(0, 3) = -dQdv.x;
		j(1, 3) = -dQdv.y;
		j(2, 3) = -dQdv.z;
		j(3, 3) = 0.5f * mat::Dot(dQdv, tangent);

		return j;
	}

	ar::mat::Vec4 Intersection::IntersectionFuncValue(ar::Entity firstObject, ar::Entity secondObject, mat::Vec4 params, ar::mat::Vec3 tangent, ar::mat::Vec3 startPoint, float stepLength)
	{
		auto p = Evaluate(firstObject, params.x, params.y);
		auto q = Evaluate(secondObject, params.z, params.w);
		auto diff = p - q;
		auto midpoint = (p + q) * 0.5;

		return {diff.x, diff.y, diff.z, mat::Dot(midpoint - startPoint, tangent) - stepLength};
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
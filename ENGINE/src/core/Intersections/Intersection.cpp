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
#include "core/Utils/CurveUtils.h"

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
		auto midpoint = 0.5f * (first->Evaluate(bestGuess.x, bestGuess.y) + second->Evaluate(bestGuess.z, bestGuess.w));

		return midpoint;
	}

	ICData Intersection::IntersectionCurve(ar::Entity firstObject, ar::Entity secondObject, float d, mat::Vec3d cursorPos, bool cursorAssisted)
	{
		ICData result;

		// =========== Preprocessing
		auto& firstDesc = firstObject.GetComponent<SurfaceComponent>().Description;
		auto& secondDesc = secondObject.GetComponent<SurfaceComponent>().Description;
		double pModifier = (firstDesc.Type == SurfaceType::RECTANGLEC0 || firstDesc.Type == SurfaceType::RECTANGLEC2) ? -1. : 1.;
		double qModifier = (secondDesc.Type == SurfaceType::RECTANGLEC0 || secondDesc.Type == SurfaceType::RECTANGLEC2) ? -1. : 1.;
		// UWAGA: pModifier nie jest w ogole uzywany, tak naprawde to w tej chwili normalsQ jest bez sensu, ale nic z tym nie bede robil!!!
		// zakladamy ze podstawka zawsze jest powierzchnia P a model powierzchnia Q
		// modifierow potrzebujemy bo dla cylindrow normalne sa na zewnatrz a dla prostokatow do wewnatrz

		// =========== Config
		const double loopCloseEpsilon = 0.01;
		double precision = 1e-4;
		size_t iterations = 15000;

		// =========== Algorithm
		Ref<ar::mat::IParametricSurface> g1, g2;
		bool selfIntersection = IsSelfIntersection(firstObject, secondObject);

		if (selfIntersection)
			g1 = g2 = Parametric::Create(firstObject);
		else
		{
			g1 = Parametric::Create(firstObject);
			g2 = Parametric::Create(secondObject);
		}

		auto newton = mat::NewtonSD(g1, g2);
		mat::Vec4d startParameter = { 0., 0., 0., 0 };

		if (cursorAssisted)
		{
			auto gc = Parametric::Create(cursorPos);
			auto p1 = StartingParams(g1, gc, false);
			auto p2 = StartingParams(g2, gc, false);
			mat::Vec4d unrefined = { p1.x, p1.y, p2.x, p2.y };

			auto cg = mat::ConjugateGradientSD(g1, g2);
			auto opt = cg.Minimize(unrefined);
			startParameter = opt.Solution;
		}
		else
		{
			startParameter = StartingParams(g1, g2, selfIntersection);	// CG minimization
		}
		
		if (selfIntersection)
		{
			float uvDistance = ((startParameter.x - startParameter.z) * (startParameter.x - startParameter.z) +
				(startParameter.y - startParameter.w) * (startParameter.y - startParameter.w));
			if (uvDistance < 0.01)
				return result;
		}

		mat::Vec4d params = startParameter, prevParams = startParameter;
		mat::Vec3d p = g1->Evaluate(params.x, params.y), q = g2->Evaluate(params.z, params.w);
		mat::Vec3d startPoint = (p + q) * 0.5;	// last "confirmed" point
		bool success = false;

		if (mat::LengthSquared(p - q) > precision) 
		{
			// p and q should be close enough to approximate them to a single point
			return result;
		}

		for (int iter = 0; iter < iterations; ++iter) 
		{
			auto newtonResult = newton.Minimize(params, startPoint, d);
			params = newtonResult.Solution;
			success = newtonResult.Converged;

			p = g1->Evaluate(params.x, params.y);
			q = g2->Evaluate(params.z, params.w);
			mat::Vec3d candidate = (p + q) * 0.5;

			// Retry (up to 5 times) if Newton failed to converge or the step was too large 
			for (int k = 0; k < 5 && (!success || mat::LengthSquared(startPoint - candidate) > precision); k++)
			{
				newtonResult = newton.Minimize(prevParams, startPoint, d * powf(0.5, k + 1));
				success = newtonResult.Converged;
				params = newtonResult.Solution;
				p = g1->Evaluate(params.x, params.y);
				q = g2->Evaluate(params.z, params.w);
				//candidate = (p + q) * 0.5;
			}

			if (!Clamp(g1, g2, params))
			{
				p = g1->Evaluate(params.x, params.y);
				q = g2->Evaluate(params.z, params.w);
				mat::Vec3d candidate = (p + q) * 0.5;

				if (result.Points.empty() || mat::LengthSquared(result.Points.back() - candidate) >= 1e-8)
				{
					result.Points.push_back(candidate);
					result.Params.push_back(params);
					auto normalP = g1->Normal(params.x, params.y);
					auto normalQ = g2->Normal(params.z, params.w);
					result.SurfaceNormalsP.push_back(normalP);
					result.SurfaceNormalsQ.push_back(normalQ);

					result.NormalsP.push_back(qModifier * ar::CurveUtils::ComputeIntCurveNormal(
						normalP, normalQ, normalP));
					result.NormalsQ.push_back(qModifier * ar::CurveUtils::ComputeIntCurveNormal(
						normalP, normalQ, normalQ));

				}
				break;
			}
			
			// Loop detection
			if (!result.Points.empty()
				&& mat::LengthSquared(candidate - result.Points[0]) < (loopCloseEpsilon * loopCloseEpsilon)
				&& iter > 10)
			{
				result.Points.push_back(result.Points[0]);
				result.Params.push_back(result.Params[0]);
				auto normalP = g1->Normal(result.Params[0].x, result.Params[0].y);
				auto normalQ = g2->Normal(result.Params[0].z, result.Params[0].w);
				result.SurfaceNormalsP.push_back(normalP);
				result.SurfaceNormalsQ.push_back(normalQ);
				result.NormalsP.push_back(qModifier * ar::CurveUtils::ComputeIntCurveNormal(
					normalP, normalQ, normalP));
				result.NormalsQ.push_back(qModifier * ar::CurveUtils::ComputeIntCurveNormal(
					normalP, normalQ, normalQ));
				return result;
			}

			if (!success || mat::LengthSquared(candidate - startPoint) > precision)
			{
				prevParams = params;
				startPoint = candidate;
				continue;
			}

			if (!result.Points.empty() && mat::LengthSquared(result.Points.back() - candidate) < 1e-8)
			{
				prevParams = params;
				startPoint = candidate;
				continue;
			}

			result.Points.push_back(candidate);
			result.Params.push_back(params);
			auto normalP = g1->Normal(params.x, params.y);
			auto normalQ = g2->Normal(params.z, params.w);
			result.SurfaceNormalsP.push_back(normalP);
			result.SurfaceNormalsQ.push_back(normalQ);
			result.NormalsP.push_back(qModifier* ar::CurveUtils::ComputeIntCurveNormal(
				normalP, normalQ, normalP));
			result.NormalsQ.push_back(qModifier* ar::CurveUtils::ComputeIntCurveNormal(
				normalP, normalQ, normalQ));

			prevParams = params;
			startPoint = candidate;
		}

		std::vector<mat::Vec3d> reverseCurve;
		std::vector<mat::Vec4d> reverseParameters;
		std::vector<mat::Vec3d> reverseSurfaceNormalsP, reverseSurfaceNormalsQ, reverseNormalsP, reverseNormalsQ;

		params = prevParams = startParameter;
		p = g1->Evaluate(params.x, params.y);
		q = g2->Evaluate(params.z, params.w);
		startPoint = (p + q) * 0.5;
		for (int i = 0; i < iterations; ++i) 
		{
			auto newtonResult = newton.Minimize(params, startPoint, -d);
			params = newtonResult.Solution;
			success = newtonResult.Converged;

			p = g1->Evaluate(params.x, params.y);
			q = g2->Evaluate(params.z, params.w);
			mat::Vec3d candidate = (p + q) * 0.5;

			for (int k = 0; k < 5 && (!success || mat::LengthSquared(candidate - startPoint) > precision); k++)
			{
				newtonResult = newton.Minimize(prevParams, startPoint, -d * powf(0.5, k + 1));
				success = newtonResult.Converged;
				params = newtonResult.Solution;
				p = g1->Evaluate(params.x, params.y);
				q = g2->Evaluate(params.z, params.w);
				//candidate = (p + q) * 0.5;
			}

			if (!Clamp(g1, g2, params))
			{
				p = g1->Evaluate(params.x, params.y);
				q = g2->Evaluate(params.z, params.w);
				mat::Vec3d candidate = (p + q) * 0.5;

				if (result.Points.empty() || mat::LengthSquared(result.Points.back() - candidate) >= 1e-8)
				{
					result.Points.push_back(candidate);
					result.Params.push_back(params);
					auto normalP = g1->Normal(params.x, params.y);
					auto normalQ = g2->Normal(params.z, params.w);
					result.SurfaceNormalsP.push_back(normalP);
					result.SurfaceNormalsQ.push_back(normalQ);
					result.NormalsP.push_back(qModifier* ar::CurveUtils::ComputeIntCurveNormal(
						normalP, normalQ, normalP));
					result.NormalsQ.push_back(qModifier* ar::CurveUtils::ComputeIntCurveNormal(
						normalP, normalQ, normalQ));

				}
				break;
			}

			if (!success || mat::LengthSquared(candidate - startPoint) > precision)
			{
				prevParams = params;
				startPoint = candidate;
				continue;
			}

			reverseCurve.push_back(candidate);
			reverseParameters.push_back(params);

			auto normalP = g1->Normal(params.x, params.y);
			auto normalQ = g2->Normal(params.z, params.w);
			reverseSurfaceNormalsP.push_back(normalP);
			reverseSurfaceNormalsQ.push_back(normalQ);
			reverseNormalsP.push_back(qModifier * ar::CurveUtils::ComputeIntCurveNormal(
				normalP, normalQ, normalP));
			reverseNormalsQ.push_back(qModifier * ar::CurveUtils::ComputeIntCurveNormal(
				normalP, normalQ, normalQ));

			prevParams = params;
			startPoint = candidate;
		}

		result.Points.insert(result.Points.begin(), reverseCurve.rbegin(), reverseCurve.rend());
		result.Params.insert(result.Params.begin(), reverseParameters.rbegin(), reverseParameters.rend());
		result.SurfaceNormalsP.insert(result.SurfaceNormalsP.begin(), reverseSurfaceNormalsP.rbegin(), reverseSurfaceNormalsP.rend());
		result.SurfaceNormalsQ.insert(result.SurfaceNormalsQ.begin(), reverseSurfaceNormalsQ.rbegin(), reverseSurfaceNormalsQ.rend());
		result.NormalsP.insert(result.NormalsP.begin(), reverseNormalsP.rbegin(), reverseNormalsP.rend());
		result.NormalsQ.insert(result.NormalsQ.begin(), reverseNormalsQ.rbegin(), reverseNormalsQ.rend());

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
			auto optimizedParams = cg.Minimize({ pair.x, pair.y, pair.z, pair.w }).Solution;
			auto s1 = first->Evaluate(optimizedParams.x, optimizedParams.y);
			auto s2 = second->Evaluate(optimizedParams.z, optimizedParams.w);
			auto optimizedDistance = mat::LengthSquared(s1 - s2);

			if (optimizedDistance < bestDistance)
			{
				if (isSelfIntersecting) 
				{
					auto dist = ((optimizedParams.x - optimizedParams.z) * (optimizedParams.x - optimizedParams.z) +
						(optimizedParams.y - optimizedParams.w) * (optimizedParams.y - optimizedParams.w));
					if (dist < 0.01) continue;
				}
				bestGuess = optimizedParams;
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

	ar::mat::Vec4i Intersection::IntersectCurves(ar::Entity first, ar::Entity second)
	{
		auto& p = first.GetComponent<IntersectCurveComponent>();
		auto& q = second.GetComponent<IntersectCurveComponent>();

		int p1 = 0, q1 = 0, p2 = 0, q2 = 0;
		float minDist = FLT_MAX;

		// phase 1: find closest points
		for (int i = 0; i < p.Points.size(); i++)
		{
			for (int j = 0; j < q.Points.size(); j++)
			{
				auto dist = ar::mat::LengthSquared(p.Points[i] - q.Points[j]);
				if (dist < minDist)
				{
					p1 = i;
					q1 = j;
					minDist = dist;
				}
			}
		}

		// phase 2: find the other point (that is not close to the first)
		const float MIN_SEPARATION = 0.1f;
		const float minSepSq = MIN_SEPARATION * MIN_SEPARATION;
		minDist = FLT_MAX;
		for (int i = 0; i < p.Points.size(); i++)
		{
			float distFromFirst = ar::mat::LengthSquared(p.Points[i] - p.Points[p1]);
			if (distFromFirst < minSepSq)
				continue;
			for (int j = 0; j < q.Points.size(); j++)
			{
				auto dist = ar::mat::LengthSquared(p.Points[i] - q.Points[j]);
				if (dist < minDist)
				{
					p2 = i;
					q2 = j;
					minDist = dist;
				}
			}
		}
		return { p1, q1, p2, q2 };
	}

	void Intersection::StitchIntersectionCurves(ar::Entity first, ar::Entity second)
	{
		auto& p = first.GetComponent<IntersectCurveComponent>();
		auto& q = second.GetComponent<IntersectCurveComponent>();

		std::vector<ar::mat::Vec3> newPoints, newNormals;

		auto intersection = IntersectCurves(first, second);
		int i1 = intersection.x; // first intersection's index in P
		int j1 = intersection.y; // first intersection's index in Q
		int i2 = intersection.z; // second intersection's index in P
		int j2 = intersection.w; // second intersection's index in Q

		int pStartIndex, pEndIndex;
		int qStartIndex, qEndIndex;

		if (i1 < i2)
		{
			// first intersection is the start for both
			pStartIndex = i1; 
			pEndIndex = i2;
			qStartIndex = j1;
			qEndIndex = j2;
		}
		else
		{
			// second intersection is the start for both
			pStartIndex = i2;
			pEndIndex = i1;
			qStartIndex = j2;
			qEndIndex = j1;
		}

		newPoints.push_back(p.Points[pStartIndex]);
		newNormals.push_back(p.NormalsP[pStartIndex]);

		// step 3: compute forward and backward vectors
		ar::mat::Vec3 forward = q.Points[(qStartIndex + 1) % q.Points.size()] - q.Points[qStartIndex];
		ar::mat::Vec3 backward = q.Points[(qStartIndex - 1) % q.Points.size()] - q.Points[qStartIndex];
		auto normal = p.NormalsP[pStartIndex];

		int k = qStartIndex, i = pEndIndex;
		if (ar::mat::Dot(normal, forward) > 0)
		{
			// move forward
			while (k != qEndIndex)
			{
				newPoints.push_back(q.Points[k]);
				newNormals.push_back(q.NormalsP[k]);
				k = (k + 1) % q.Points.size();
			}
		}
		else
		{
			// move backward
			while (k != qEndIndex)
			{
				newPoints.push_back(q.Points[k]);
				newNormals.push_back(q.NormalsP[k]);
				k = (k - 1 + q.Points.size()) % q.Points.size();
			}
		}

		// step 4: add points until start is reached
		while (i != pStartIndex)
		{
			newPoints.push_back(p.Points[i]);
			newNormals.push_back(p.NormalsP[i]);
			i = (i + 1) % p.Points.size();
		}

		// step 5: modify first curve (base)
		p.Points = newPoints;
		p.NormalsP = newNormals;
		p.DirtyFlag = true;
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
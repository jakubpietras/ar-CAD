#include "conjugateGradient.h"

namespace ar::mat
{
	ConjugateGradient4D::ConjugateGradient4D(std::shared_ptr<IParametricSurface> first, std::shared_ptr<IParametricSurface> second)
		: m_First(first), m_Second(second)
	{ }

	CGResult ConjugateGradient4D::Minimize(const Vec4d & initialGuess, const CGConfig & config)
	{
		mat::Vec4d direction, params, gradient, prevGradient;
		mat::Vec3d p, q;
		double phi0 = 0., slope = 0., alpha = 0.;
		CGResult result{};
		LineSearchResult lsResult{};
		
		// ================= Initialization
		params = initialGuess;
		gradient = prevGradient = GradientFunction(params);
		direction = -gradient;
		phi0 = ObjectiveFunction(params);
		slope = mat::Dot(gradient, direction); // ??
		auto squaredDistancePhi = [&](double alpha) 
			{
				ar::mat::Vec4d candidate = params + direction * alpha;
				ClampFunction(candidate);

				auto p1 = m_First->Evaluate(candidate.x, candidate.y);
				auto p2 = m_Second->Evaluate(candidate.z, candidate.w);
				return mat::LengthSquared(p1 - p2);
			};

		// ================= Algorithm
		lsResult = LineSearch1D::FindStepSize(squaredDistancePhi, phi0, slope);
		if (!lsResult.Success)
			return result;
		alpha = lsResult.StepSize;
		params = params + direction * alpha;
		ClampFunction(params);

		for (size_t iter = 0; iter < config.MaxIterations; iter++)
		{
			// 'params' updated at the end of previous iteration:
			prevGradient = gradient;
			gradient = GradientFunction(params);
			if (mat::LengthSquared(gradient) < config.Tolerance * config.Tolerance)
			{
				result.Solution = params;
				result.Converged = true;
				result.Iterations = iter + 1;
				break;
			}

			// find search direction
			double beta = PolakRibiere(prevGradient, gradient);
			if (iter % 20 == 0) beta = 0.0;
			direction = -gradient + direction * beta;

			// find step size towards that direction
			phi0 = ObjectiveFunction(params);
			slope = mat::Dot(gradient, direction);
			lsResult = LineSearch1D::FindStepSize(squaredDistancePhi, phi0, slope);
			if (!lsResult.Success) 
			{
				// fallback: steepest descent
				direction = -gradient;
				slope = Dot(gradient, direction);
				lsResult = LineSearch1D::FindStepSize(squaredDistancePhi, phi0, slope);
				if (!lsResult.Success) break;
			}

			// update 'params' based on the calculations
			alpha = lsResult.StepSize;
			params = params + direction * alpha;
			ClampFunction(params);
		}
		result.Solution = params;
		result.Iterations = config.MaxIterations;
		result.Converged = false;
		return result;
	}

	double ConjugateGradient4D::ObjectiveFunction(const Vec4d& params)
	{
		// f(u, v, s, t) = |P(u,v) - Q(s,t)|^2
		// Squared Distance between two parametric surfaces
		auto p = m_First->Evaluate(params.x, params.y);
		auto q = m_Second->Evaluate(params.z, params.w);
		return mat::LengthSquared(p - q);
	}

	Vec4d ConjugateGradient4D::GradientFunction(const Vec4d& params)
	{
		// f(u, v, s, t) = |P(u,v) - Q(s,t)|^2
		auto u = params.x, v = params.y, s = params.z, t = params.w;
		auto difference = m_First->Evaluate(u, v) - m_Second->Evaluate(s, t);

		auto dPdu = m_First->DerivativeU(u, v), dPdv = m_First->DerivativeV(u, v);
		auto dQdu = m_Second->DerivativeU(s, t), dQdv = m_Second->DerivativeV(s, t);

		return {
			2 * mat::Dot(difference, dPdu),
			2 * mat::Dot(difference, dPdv),
			-2 * mat::Dot(difference, dQdu),
			-2 * mat::Dot(difference, dQdv)
		};
	}

	bool ConjugateGradient4D::ClampFunction(Vec4d& params)
	{
		auto c1 = m_First->Clamp(params.x, params.y);
		auto c2 = m_Second->Clamp(params.z, params.w);
		return c1 && c2;
	}

	double ConjugateGradient4D::PolakRibiere(const Vec4d& prevGradient, const Vec4d& currGradient)
	{
		mat::Vec4d y = currGradient - prevGradient;
		double denom = mat::LengthSquared(prevGradient);
		double beta = 0.0;
		if (denom > 0.0)
		{
			beta = mat::Dot(currGradient, y) / denom;
			if (beta < 0.0) beta = 0.0; // reset if negative (PR+)
		}
		return beta;
	}
}
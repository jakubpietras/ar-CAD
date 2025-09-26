#include "conjugateGradient.h"

namespace ar::mat
{
	ConjugateGradientSD::ConjugateGradientSD(std::shared_ptr<IParametricSurface> first, std::shared_ptr<IParametricSurface> second)
		: m_First(first), m_Second(second)
	{ }

	CGResult ConjugateGradientSD::Minimize(const Vec4d & initialGuess, const CGConfig & config)
	{
		mat::Vec4d xp = initialGuess;
		mat::Vec4d xn = initialGuess;
		mat::Vec4d grad = Gradient(xp);
		CGResult result{};

		auto squaredDistancePhi = [&](double alpha, mat::Vec4d params, mat::Vec4d direction)
			{
				ar::mat::Vec4d candidate = params + direction * alpha;
				Clamp(candidate);
				return ObjectiveFunction(candidate);
			};

		// Calculate the steepest direction
		mat::Vec4d sp = -grad, sn = -grad, dxn = -grad, dxp = -grad;
		double dp = mat::Dot(grad, grad);
		double dn = dp;
		double alpha = LineSearch1D::FindStepSizeCG(squaredDistancePhi,
			ObjectiveFunction(xp), mat::Dot(grad, sp), xp, sp).StepSize;
		xn = xp + dxp * alpha;
		Clamp(xn);

		for (int iter = 0; iter < config.MaxIterations; ++iter) 
		{
			if (dn < config.Tolerance)
			{
				result.Converged = true;
				result.Iterations = iter + 1;
				result.Solution = xn;
				break;
			}

			// Calculate the steepest direction :
			dxn = -Gradient(xn);

			// Compute beta
			// dn       = dot(sn, sn);
			double b = mat::Dot(dxn, dxn) / mat::Dot(dxp, dxp);

			// Update the conjugate direction:
			sn = dxn + sp * b;

			// compute beta
			dn = mat::Dot(dxn, dxn);
			b = dn / dp;

			if (iter % 20 == 0) {
				sn = sp = -grad;
				b = 0.0;
			}

			// Update the conjugate direction
			sn = dxn + sp * b;

			// perform linear search
			alpha = LineSearch1D::FindStepSizeCG(squaredDistancePhi,
				ObjectiveFunction(xp), mat::Dot(-dxn, sn), xn, sn).StepSize;

			// Update the position:
			xn = xn + sn * alpha;

			if (!Clamp(xn)) {
				result.Converged = false;
				result.Solution = { 0., 0., 0. };
				return result;
			}

			xp = xn;
			sp = sn;
			dp = dn;
		}
		return result;

	}

	double ConjugateGradientSD::ObjectiveFunction(const Vec4d& params)
	{
		// f(u, v, s, t) = |P(u,v) - Q(s,t)|^2
		// Squared Distance between two parametric surfaces
		auto p = m_First->Evaluate(params.x, params.y);
		auto q = m_Second->Evaluate(params.z, params.w);
		return mat::LengthSquared(p - q);
	}

	Vec4d ConjugateGradientSD::Gradient(const Vec4d& params)
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

	bool ConjugateGradientSD::Clamp(Vec4d& params)
	{
		auto c1 = m_First->Clamp(params.x, params.y);
		auto c2 = m_Second->Clamp(params.z, params.w);
		return c1 && c2;
	}

	double ConjugateGradientSD::PolakRibiere(const Vec4d& prevGradient, const Vec4d& currGradient)
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
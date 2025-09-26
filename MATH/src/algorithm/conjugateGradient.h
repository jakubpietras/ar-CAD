#pragma once
#include <functional>
#include "vector_types.h"
#include <memory>
#include "parametric/parametricSurface.h"
#include "lineSearch.h"

namespace ar::mat
{
	struct CGResult
	{
		Vec4d	Solution = {};
		bool	Converged = false;
		size_t	Iterations = 0;
		double	FinalGradientNorm = 0.0;
	};

	struct CGConfig
	{
		double	Tolerance = 1e-8;
		size_t	MaxIterations = 50;
	};

	class ConjugateGradient4D
	{
	public:
		ConjugateGradient4D(std::shared_ptr<IParametricSurface> first,
			std::shared_ptr<IParametricSurface> second);
		CGResult Minimize(const Vec4d& initialGuess, const CGConfig& config = {});

	private:
		std::shared_ptr<IParametricSurface> m_First, m_Second;

		double ObjectiveFunction(const Vec4d& params);	// Squared Distance function
		Vec4d GradientFunction(const Vec4d& params);	// of Squared Distance function
		bool ClampFunction(Vec4d& params);				// clamp resulting params for the objects				
		double PolakRibiere(const Vec4d& prevGradient, const Vec4d& currGradient);
	};
}
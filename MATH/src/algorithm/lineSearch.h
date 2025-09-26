#pragma once
#include <functional>
#include "vector_types.h"
#include <memory>
#include "parametric/parametricSurface.h"

namespace ar::mat
{
	struct LineSearchResult
	{
		double		StepSize;
		bool		Success;
		size_t		FunctionEvaluations;
	};

	struct LineSearchConfig
	{
		double		ArmijoParameter = 1e-4;
		double		ShrinkFactor = 0.5;
		double		InitialStep = 1.0;
		double		MinStep = 1e-8;
		size_t		MaxEvaluations = 20;
	};

	class LineSearch1D
	{
	public:
		using ScalarFunction = std::function<double(double)>;
		static LineSearchResult FindStepSize(ScalarFunction phi, double phi0, double slope0,
			const LineSearchConfig& config = {});
	};
}
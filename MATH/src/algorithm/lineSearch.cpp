#include "lineSearch.h"

namespace ar::mat
{
    LineSearchResult LineSearch1D::FindStepSize(ScalarFunction phi, double phi0, double slope0,
        const LineSearchConfig& config)
    {
        if (slope0 >= 0.)
            return { config.MinStep, false, 0 };

        double alpha = config.InitialStep;
        size_t evaluations = 0;

        for (size_t iter = 0; iter < config.MaxEvaluations; iter++)
        {
            double phiAlpha = phi(alpha);
            evaluations++;

            // Armijo condition
            if (phi0 - phiAlpha >= -alpha * config.ArmijoParameter * slope0)
                return { alpha, true, evaluations };
            
            alpha *= config.ShrinkFactor;
            if (alpha < config.MinStep)
                break;
        }

        return { alpha, false, evaluations };
    }
}


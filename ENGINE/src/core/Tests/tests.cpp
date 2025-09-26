#include "arpch.h"
#include "tests.h"
#include "algorithm/lineSearch.h"

namespace ar
{
    void Tests::TestLineSearchSuite()
    {
        AR_TRACE("===== Running Line Search Test Suite =====");
        TestLineSearch_SuccessAndNonDescent();
        TestLineSearch_FailureDueToMaxEvaluations();
        TestLineSearch_FailureDueToMinStep();
        AR_TRACE("===== Line Search Test Suite Complete =====");
    }
    void Tests::TestLineSearch_SuccessAndNonDescent()
	{
        double x = 1.0;
        double p = -2.0;
        auto phi = [=](double alpha) {
            double val = (x + alpha * p);
            return val * val;
            };

        double phi0 = phi(0.0);                   
        double slope0 = (2 * x) * p;              
        ar::mat::LineSearchConfig config;                  

        AR_TRACE("Testing backtracking line search...");
        AR_INFO("Initial f(x)={0} slope={1}", phi0, slope0);

        auto result = ar::mat::LineSearch1D::FindStepSize(phi, phi0, slope0, config);

        if (result.Success) {
            AR_INFO("Line search succeeded.");
            AR_INFO("Step size = {0}, function evaluations = {1}",
                result.StepSize, result.FunctionEvaluations);
        }
        else {
            AR_ERROR("Line search FAILED.");
            AR_ERROR("Step size = {0}, evaluations = {1}",
                result.StepSize, result.FunctionEvaluations);
        }

        double badSlope = +1.0;
        result = ar::mat::LineSearch1D::FindStepSize(phi, phi0, badSlope, config);

        if (!result.Success) {
            AR_INFO("Correctly failed with non-descent direction.");
            AR_INFO("Reported step {0}, evaluations = {1}",
                result.StepSize, result.FunctionEvaluations);
        }
        else {
            AR_ERROR("Unexpected success with non-descent direction!");
        }
	}

    void Tests::TestLineSearch_FailureDueToMinStep()
    {
        double x = 1.0;
        double p = -2.0;
        auto phi = [=](double alpha) {
            double val = (x + alpha * p);
            return val * val;
            };

        double phi0 = phi(0.0);
        double slope0 = (2 * x) * p; // -4

        ar::mat::LineSearchConfig config;
        config.InitialStep = 1.0;
        config.ShrinkFactor = 0.5;
        config.MinStep = 0.05;         // artificially cut at 0.05
        config.ArmijoParameter = 0.99; // requires  0.01 to succeed
        config.MaxEvaluations = 50;

        AR_TRACE("Testing line search with strict Armijo + MinStep contradiction...");
        AR_INFO("Initial f(x)={0} slope={1}", phi0, slope0);

        auto result = ar::mat::LineSearch1D::FindStepSize(phi, phi0, slope0, config);

        if (!result.Success) {
            AR_INFO("Correctly failed by hitting MinStep cutoff.");
            AR_INFO("Reported step {0}, evaluations = {1}",
                result.StepSize, result.FunctionEvaluations);
        }
        else {
            AR_ERROR("Unexpected success — was supposed to fail due to MinStep limit!");
        }
    }

    void Tests::TestLineSearch_FailureDueToMaxEvaluations()
    {
        double x = 1.0;
        double p = -2.0;
        auto phi = [=](double alpha) {
            double val = (x + alpha * p);
            return val * val;
            };

        double phi0 = phi(0.0);
        double slope0 = (2 * x) * p;   // -4

        ar::mat::LineSearchConfig config;
        config.MaxEvaluations = 1;     // force fail by cutting off too early
        config.InitialStep = 1.0;      // initial wont satisfy Armijo

        AR_TRACE("Testing line search with MaxEvaluations=1 (forced fail)...");
        AR_INFO("Initial f(x)={0} slope={1}", phi0, slope0);

        auto result = ar::mat::LineSearch1D::FindStepSize(phi, phi0, slope0, config);

        if (!result.Success) {
            AR_INFO("Correctly failed due to MaxEvaluations cutoff.");
            AR_INFO("Reported step {0}, evaluations = {1}",
                result.StepSize, result.FunctionEvaluations);
        }
        else {
            AR_ERROR("Unexpected success — was supposed to fail due to MaxEvaluations limit!");
        }
    }
}
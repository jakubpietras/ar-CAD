#pragma once

namespace ar
{
	class Tests
	{
	public:
		static void TestLineSearchSuite();


		static void TestLineSearch_SuccessAndNonDescent();
		static void TestLineSearch_FailureDueToMinStep();
		static void TestLineSearch_FailureDueToMaxEvaluations();
	};
}
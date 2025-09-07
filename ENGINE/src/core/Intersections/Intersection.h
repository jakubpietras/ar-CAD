#pragma once
#include "core/Scene/Entity.h"

namespace ar
{
	class Intersection
	{
	public:
		// 'main' function will return data for the intersection curve
		static mat::Vec3 FindStartingPointDebug(ar::Entity firstObject, ar::Entity secondObject);

	private:
		// Preliminary search for best first guess:
		static mat::Vec4 CalculateStartingParams(ar::Entity firstObject, ar::Entity secondObject, size_t samples);
		static std::vector<mat::Vec4> GenerateUVPairs(size_t samples);
		static float ComputeRealDistance(ar::Entity firstObject, ar::Entity secondObject, mat::Vec4 params);
		static mat::Vec4 CGSquaredDistance(ar::Entity firstObject, ar::Entity secondObject, mat::Vec4 initial);
		static float LineSearchSquaredDistance(ar::Entity firstObject, ar::Entity secondObject, ar::mat::Vec4 params, ar::mat::Vec4 direction);
		static float SquaredDistanceValue(ar::Entity firstObject, ar::Entity secondObject, ar::mat::Vec4 params);
		static void ClampWrapUV(ar::Entity obj, float& u, float& v);
		static void ClampWrapObjects(ar::Entity o1, ar::Entity o2, ar::mat::Vec4& p);
		static float PolakRibiere(mat::Vec4 prevGrad, mat::Vec4 currGrad);

		// Tracing the intersection curve:
		static mat::Vec4 NewtonMinimization(ar::Entity firstObject, ar::Entity secondObject, mat::Vec4 initial);
		

		// --------- 
		static mat::Vec3 DerivativeU(ar::Entity object, float u, float v);
		static mat::Vec3 DerivativeV(ar::Entity object, float u, float v);
		static mat::Vec4 SquaredDistanceGradient(ar::Entity firstObject, ar::Entity secondObject, mat::Vec4 params);
		static mat::Vec3 Evaluate(ar::Entity object, float u, float v);
		static void TestGradient(ar::Entity obj1, ar::Entity obj2);

	};
}


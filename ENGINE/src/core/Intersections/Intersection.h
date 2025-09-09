#pragma once
#include "core/Scene/Entity.h"

namespace ar
{
	struct SegmentInfo {
		ar::mat::Vec2 segment;      // Which segment (integer indices)
		ar::mat::Vec2 localParams;  // Local parameters within that segment [0,1]
		ar::mat::Vec2 scaling;      // Scaling factors for derivatives (chain rule)
	};

	class Intersection
	{
	public:
		// 'main' function will return data for the intersection curve
		static mat::Vec3 FindStartingPoint(ar::Entity firstObject, ar::Entity secondObject);

	private:
		// Preliminary search for best first guess:
		static mat::Vec4 CalculateStartingParams(ar::Entity firstObject, ar::Entity secondObject, size_t samples);
		static std::vector<mat::Vec4> GenerateUVPairs(size_t samples);
		static float ComputeRealDistance(ar::Entity firstObject, ar::Entity secondObject, mat::Vec4 params);
		static mat::Vec4 CGSquaredDistance(ar::Entity firstObject, ar::Entity secondObject, mat::Vec4 initial);
		static float LineSearchSquaredDistance(ar::Entity firstObject, ar::Entity secondObject, ar::mat::Vec4 params, ar::mat::Vec4 direction);
		static float SquaredDistanceValue(ar::Entity firstObject, ar::Entity secondObject, ar::mat::Vec4 params);
		static bool ClampWrapUV(ar::Entity obj, float& u, float& v);
		static bool ClampWrapObjects(ar::Entity o1, ar::Entity o2, ar::mat::Vec4& p);
		static float PolakRibiere(mat::Vec4 prevGrad, mat::Vec4 currGrad);
		static SegmentInfo MapMultipatch(ar::Entity patch, float u, float v);

		// Tracing the intersection curve:
		static bool NewtonMinimization(ar::mat::Vec4& result, ar::Entity firstObject, ar::Entity secondObject, mat::Vec4 initial, double distance);
		static mat::Mat4 IntersectionFuncJacobian(ar::Entity firstObject, ar::Entity secondObject, mat::Vec4 params, ar::mat::Vec3 tangent);
		static mat::Vec4 IntersectionFuncValue(ar::Entity firstObject, ar::Entity secondObject, 
			mat::Vec4 params, ar::mat::Vec3 tangent, ar::mat::Vec3 startPoint, float stepLength);

		// --------- 
		static mat::Vec3 DerivativeU(ar::Entity object, float u, float v);
		static mat::Vec3 DerivativeV(ar::Entity object, float u, float v);
		static mat::Vec4 SquaredDistanceGradient(ar::Entity firstObject, ar::Entity secondObject, mat::Vec4 params);
		static mat::Vec3 Evaluate(ar::Entity object, float u, float v);

	};
}


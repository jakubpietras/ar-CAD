#pragma once
#include "core/Scene/Entity.h"
#include "parametric/parametricSurface.h"

namespace ar
{
	struct ICData
	{
		std::vector<mat::Vec3d> Points;
		std::vector<mat::Vec4d> Params;
		std::vector<mat::Vec3d> SurfaceNormalsP, SurfaceNormalsQ;
		std::vector<mat::Vec3d> NormalsP, NormalsQ;
	};

	class Intersection
	{
	public:
		static mat::Vec3d FindStartingPoint(ar::Entity firstObject, ar::Entity secondObject);
		static ICData IntersectionCurve(ar::Entity firstObject, ar::Entity secondObject, float d, mat::Vec3d cursorPos, bool cursorAssisted = false);
		static mat::Vec4d StartingParams(Ref<mat::IParametricSurface> first, Ref<mat::IParametricSurface> second, bool isSelfIntersecting);
		static std::vector<mat::Vec4d> GenerateUVs(size_t samples, bool selfIntersect);
		static void DrawDerivatives(ar::Entity object, size_t samples);
		static void DrawEvaluations(ar::Entity object, size_t samples);

		static mat::Vec4i IntersectCurves(ar::Entity first, ar::Entity second);
		static void StitchIntersectionCurves(ar::Entity first, ar::Entity second);

	private:
		static bool Clamp(Ref<mat::IParametricSurface> first, Ref<mat::IParametricSurface> second, mat::Vec4d& params);
		static std::vector<mat::Vec4> GenerateUVPairs(size_t samples, bool selfIntersect);
		static bool IsSelfIntersection(ar::Entity first, ar::Entity second);
	};
}


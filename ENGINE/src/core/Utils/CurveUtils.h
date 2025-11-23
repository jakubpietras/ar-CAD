#pragma once
#include "core/Scene/Entity.h"
#include "core/Scene/SceneFactory.h"

namespace ar
{
	class CurveUtils
	{
	public:
		static bool ValidateChain(Entity& entity);
		static bool ValidateBezier(Entity& entity);

		static bool ValidateCurve(Entity& entity);
		static std::vector<uint32_t> GenerateC0Indices(size_t pointCount);
		static std::vector<uint32_t> GenerateC2Indices(size_t pointCount);

		static std::vector<VertexPositionID> GetIntC2VertexData(std::vector<ar::Entity> knots, uint32_t id);

		static std::array<std::array<mat::Vec3, 4>, 2> SubdivideCubicSegment(std::array<mat::Vec3, 4> controlPoints, float t);

		static std::vector<ar::mat::Vec3> ComputeIntCurveNormals(std::vector<ar::mat::Vec3> surfaceNormalsP, std::vector<ar::mat::Vec3> surfaceNormalsQ);
		static ar::mat::Vec3d ComputeIntCurveNormal(ar::mat::Vec3d surfaceNormalP, ar::mat::Vec3d surfaceNormalQ, ar::mat::Vec3d projSurfNormal);
		static void ConvertIntersectCurve(ar::Entity intersectCurve, ar::SceneFactory& factory);
		static void ResizeIntersectCurve(ar::Entity intersectCurve);
		static std::vector<ar::mat::Vec3> FlattenNormals(const std::vector<ar::mat::Vec3>& normals);

	private:
		static std::vector<mat::Vec3> FilterKnots(std::vector<mat::Vec3> knots);
		static std::vector<float> ComputeChordLengths(std::vector<ar::mat::Vec3> knots);
		static std::vector<VertexPositionID> ConvertCoeffToBezier(std::vector<mat::Vec3> coeffA,
			std::vector<mat::Vec3> coeffB, std::vector<mat::Vec3> coeffC,
			std::vector<mat::Vec3> coeffD, std::vector<float> chordLengths, uint32_t id);
	};
}
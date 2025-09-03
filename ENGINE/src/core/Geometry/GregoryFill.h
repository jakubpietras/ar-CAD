#pragma once
#include "core/Geometry/HoleDetector.h"

namespace ar
{
	class GregoryFill
	{
	public:
		static std::vector<VertexPositionID> GetGregoryVertexData(ar::Hole& holeToFill, uint32_t id);

	private:
		static std::vector<std::array<mat::Vec3, 20>> G;
		static std::vector<std::array<mat::Vec3, 8>> B;
		static std::vector<std::array<mat::Vec3, 6>> R;
		static std::vector<std::array<mat::Vec3, 4>> S;
		static std::vector<std::array<mat::Vec3, 2>> T;
		static std::array<mat::Vec3, 3> Q;
		static std::vector<std::array<mat::Vec3, 4>> P;

		static void ComputeGregoryPoints();
		static void StoreBezierPoints(std::vector<ar::EdgeInfo> edges);
		static void ComputeSubdivisionPoints(std::vector<ar::EdgeInfo> edges);
		static void ComputeInternalPoints(std::vector<ar::EdgeInfo> edges);


	};
}

#include "arpch.h"
#include "GregoryFill.h"
#include "core/Utils/GeneralUtils.h"

namespace ar
{
	std::vector<std::array<mat::Vec3, 20>>  GregoryFill::G;
	std::vector<std::array<mat::Vec3, 8>>	GregoryFill::B;
	std::vector<std::array<mat::Vec3, 6>>	GregoryFill::R;
	std::vector<std::array<mat::Vec3, 4>>	GregoryFill::S;
	std::vector<std::array<mat::Vec3, 2>>	GregoryFill::T;
	std::array<mat::Vec3, 3>				GregoryFill::Q;
	std::vector<std::array<mat::Vec3, 4>>	GregoryFill::P;

	void GregoryFill::ComputeGregoryPoints()
	{
		G.clear(); G.resize(3);

		for (int edge = 0; edge < 3; edge++)
		{
			int next = (edge + 1) % 3;
			G[edge][0] = P[edge][3];
			G[edge][1] = P[edge][2];
			G[edge][2] = P[edge][1];
			G[edge][3] = P[edge][0];
			G[edge][4] = S[edge][1];
			G[edge][5] = G[edge][4] + (S[edge][1] - S[edge][3]);
			G[edge][6] = G[edge][5];
			G[edge][7] = G[edge][5] + (G[edge][2] - G[edge][1]);
			G[edge][9] = P[next][1];
			G[edge][15] = P[next][2];
			G[edge][10] = R[edge][2];
			G[edge][11] = G[edge][10] + (R[edge][2] - R[edge][5]);
			G[edge][16] = B[edge][3];
			G[edge][17] = R[next][0];
			G[edge][18] = S[next][0];
			G[edge][19] = P[next][3];
			G[edge][13] = G[edge][18] + (S[next][0] - S[next][2]);
			G[edge][14] = G[edge][13];
			G[edge][8] = G[edge][13] + (G[edge][9] - G[edge][15]);
			G[edge][12] = G[edge][17] + (R[next][0] - R[next][3]);
		}
	}

	void GregoryFill::StoreBezierPoints(std::vector<ar::EdgeInfo> edges)
	{
		B.clear();
		B.resize(3);
		for (int edge = 0; edge < 3; edge++)
		{
			auto points = GeneralUtils::GetPos(edges[edge].Points);
			auto neighbors = GeneralUtils::GetPos(edges[edge].Neighbors);
			for (int i = 0; i < 4; i++)
			{
				B[edge][i] = points[i];
				B[edge][i + 4] = neighbors[i];
			}
		}
	}

	void GregoryFill::ComputeSubdivisionPoints(std::vector<ar::EdgeInfo> edges)
	{
		R.clear(); R.resize(3);
		S.clear(); S.resize(3);
		T.clear(); T.resize(3);

		// R - first lerp based on Bezier points
		for (int edge = 0; edge < 3; edge++)
		{
			R[edge][0] = mat::Lerp(B[edge][0], B[edge][1], 0.5f);
			R[edge][1] = mat::Lerp(B[edge][1], B[edge][2], 0.5f);
			R[edge][2] = mat::Lerp(B[edge][2], B[edge][3], 0.5f);
			R[edge][3] = mat::Lerp(B[edge][4], B[edge][5], 0.5f);
			R[edge][4] = mat::Lerp(B[edge][5], B[edge][6], 0.5f);
			R[edge][5] = mat::Lerp(B[edge][6], B[edge][7], 0.5f);
		}

		// S - second lerp based on points in R
		for (int edge = 0; edge < 3; edge++)
		{
			S[edge][0] = mat::Lerp(R[edge][0], R[edge][1], 0.5f);
			S[edge][1] = mat::Lerp(R[edge][1], R[edge][2], 0.5f);
			S[edge][2] = mat::Lerp(R[edge][3], R[edge][4], 0.5f);
			S[edge][3] = mat::Lerp(R[edge][4], R[edge][5], 0.5f);
		}

		// T - third lerp based on points in S
		for (int edge = 0; edge < 3; edge++)
		{
			T[edge][0] = mat::Lerp(S[edge][0], S[edge][1], 0.5f);
			T[edge][1] = mat::Lerp(S[edge][2], S[edge][3], 0.5f);
		}
	}

	void GregoryFill::ComputeInternalPoints(std::vector<ar::EdgeInfo> edges)
	{
		P.clear(); P.resize(3);

		for (int edge = 0; edge < 3; edge++)
		{
			P[edge][3] = T[edge][0];
			P[edge][2] = P[edge][3] + (T[edge][0] - T[edge][1]);
			//Q[edge] = (3 * P[edge][2] - P[edge][3]) / 2;
			Q[edge] = P[edge][2];
		}

		mat::Vec3 P0 = (Q[0] + Q[1] + Q[2]) / 3;

		for (int edge = 0; edge < 3; edge++)
		{
			P[edge][1] = (2 * Q[edge] + P0) / 3;
			P[edge][0] = P0;
		}
	}

	std::vector<ar::VertexPositionID> GregoryFill::GetGregoryVertexData(ar::Hole& holeToFill, uint32_t id)
	{
		std::vector<ar::EdgeInfo> E = { holeToFill.Edges[0] , holeToFill.Edges[1], holeToFill.Edges[2] };
		StoreBezierPoints(E);
		ComputeSubdivisionPoints(E);
		ComputeInternalPoints(E);
		ComputeGregoryPoints();

		std::vector<ar::VertexPositionID> verts;
		for (int edge = 0; edge < 3; edge++)
		{
			for (int point = 0; point < 20; point++)
			{
				verts.push_back({ G[edge][point], id });
			}
		}
		return verts;
	}

	std::vector<uint32_t> GregoryFill::GetGregoryControlMesh()
	{
		std::vector<uint32_t> indices;
		for (int segment = 0; segment < 4; segment++)
		{
			uint32_t offset = segment * 20;
			indices.insert(indices.begin(), {
				offset + 0, offset + 1,
				offset + 1, offset + 5,
				offset + 1, offset + 2,
				offset + 2, offset + 7,
				offset + 2, offset + 3,
				offset + 3, offset + 9,
				offset + 9, offset + 8,
				offset + 9, offset + 15,
				offset + 15, offset + 13,
				offset + 15, offset + 19,
				offset + 19, offset + 18,
				offset + 18, offset + 13,
				offset + 18, offset + 17,
				offset + 17, offset + 12,
				offset + 17, offset + 16,
				offset + 16, offset + 10,
				offset + 10, offset + 11,
				offset + 10, offset + 4,
				offset + 4, offset + 5,
				offset + 4, offset + 0
			});
		}
		return indices;
	}

}
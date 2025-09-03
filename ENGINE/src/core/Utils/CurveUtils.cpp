#include "arpch.h"
#include "CurveUtils.h"
#include "solvers.h"
#include "core/Scene/Components.h"

namespace ar
{
	bool CurveUtils::ValidateChain(Entity& entity)
	{
		auto& points = entity.GetComponent<ControlPointsComponent>().Points;
		return points.size() > 1;
	}

	bool CurveUtils::ValidateBezier(Entity& entity)
	{
		auto& points = entity.GetComponent<ControlPointsComponent>().Points;
		return points.size() > 1;
	}

	bool CurveUtils::ValidateCurve(Entity& entity)
	{
		auto& points = entity.GetComponent<ControlPointsComponent>().Points;
		return points.size() > 1;
	}

	std::vector<uint32_t> CurveUtils::GenerateC0Indices(size_t pointCount)
	{
		size_t index = 0;
		std::vector<uint32_t> indices;
		while (index < pointCount)
		{
			auto remaining = pointCount - index;
			uint32_t i = static_cast<uint32_t>(index);
			if (remaining > 3)
			{
				indices.insert(indices.end(), { i, i + 1, i + 2, i + 3 });
				index += 3;
			}
			else if (remaining > 2)
			{
				indices.insert(indices.end(), { i, i + 1, i + 2, i + 2 });
				break;
			}
			else if (remaining > 1)
			{
				indices.insert(indices.end(), { i, i, i + 1, i + 1 });
				break;
			}
			else
				break;
		}
		return indices;
	}

	std::vector<uint32_t> CurveUtils::GenerateC2Indices(size_t pointCount)
	{
		size_t index = 0;
		std::vector<uint32_t> indices;
		while (index < pointCount)
		{
			auto remaining = pointCount - index;
			uint32_t i = static_cast<uint32_t>(index);
			if (remaining > 3)
			{
				indices.insert(indices.end(), { i, i + 1, i + 2, i + 3 });
				index++;
			}
			else if (remaining > 2)
			{
				indices.insert(indices.end(), { i, i + 1, i + 2, i + 2 });
				break;
			}
			else if (remaining > 1)
			{
				indices.insert(indices.end(), { i, i, i + 1, i + 1 });
				break;
			}
			else
				break;
		}
		return indices;
	}

	std::vector<VertexPositionID> CurveUtils::GetIntC2VertexData(std::vector<ar::Entity> knots, uint32_t id)
	{
		// build and solve matrix
		std::vector<float> lower, upper, main, chordLengths;
		std::vector<mat::Vec3> rhs, knotPos, coeffA, coeffB, coeffC, coeffD;
		
		std::transform(knots.begin(), knots.end(), std::back_inserter(knotPos),
			[](ar::Entity& e) {
				return e.GetComponent<ar::TransformComponent>().Translation;
			});
		knotPos = FilterKnots(knotPos);
		chordLengths = ComputeChordLengths(knotPos);

		if (knots.size() == 2)
		{
			coeffA.resize(1);
			coeffB.resize(1);
			coeffC.resize(1);
			coeffD.resize(1);

			mat::Vec3 c0{0, 0, 0}, c1{ 0, 0, 0 };

			coeffA[0] = knotPos[0];
			coeffC[0] = c0;
			coeffD[0] = mat::Vec3(0, 0, 0);
			coeffB[0] = (knotPos[1] - knotPos[0]) / chordLengths[0];
		}
		else
		{
			for (int i = 1; i < knotPos.size() - 1; i++)
			{
				float alpha = chordLengths[i - 1] / (chordLengths[i - 1] + chordLengths[i]);
				float beta = chordLengths[i] / (chordLengths[i - 1] + chordLengths[i]);
				lower.push_back(alpha);
				upper.push_back(beta);
				main.push_back(2.0f);
				auto diff_prev = (knotPos[i] - knotPos[i - 1]) / chordLengths[i - 1];
				auto diff_next = (knotPos[i + 1] - knotPos[i]) / chordLengths[i];
				rhs.push_back(3.0f * (diff_next - diff_prev) / (chordLengths[i - 1] + chordLengths[i]));
			}

			auto cInterior = ar::mat::SolveTridiagonal(lower, main, upper, rhs);
			std::vector<mat::Vec3> c(knotPos.size(), mat::Vec3(0, 0, 0));
			for (int i = 0; i < cInterior.size(); i++)
				c[i + 1] = cInterior[i];

			coeffA.resize(knotPos.size() - 1);
			coeffB.resize(knotPos.size() - 1);
			coeffC.resize(knotPos.size() - 1);
			coeffD.resize(knotPos.size() - 1);
			for (int i = 0; i < knots.size() - 1; i++)
			{
				coeffA[i] = knotPos[i];
				coeffC[i] = c[i];
				coeffD[i] = (c[i + 1] - c[i]) / (3.0f * chordLengths[i]);
				coeffB[i] = (knotPos[i + 1] - knotPos[i]) / chordLengths[i] -
					chordLengths[i] * (2.0f * c[i] + c[i + 1]) / 3.0f;
			}
		}
		return ConvertCoeffToBezier(coeffA, coeffB, coeffC, coeffD, chordLengths, id);
	}

	std::array<std::array<mat::Vec3, 4>, 2> CurveUtils::SubdivideCubicSegment(std::array<mat::Vec3, 4> controlPoints, float t)
	{
		std::vector<mat::Vec3> points, buffer, result;

		points.assign(controlPoints.begin(), controlPoints.end());
		for (int end = 4; end > 1; end--)
		{
			for (int start = 0; start < end - 1; start++)
			{
				auto point = mat::Lerp(points[start], points[start + 1], t);
				buffer.push_back(point);
				result.push_back(point);
			}
			points = buffer;
			buffer.clear();
		}

		std::array<mat::Vec3, 4> curveA = {controlPoints[0], result[0], result[3], result[5]};
		std::array<mat::Vec3, 4> curveB = { result[5], result[4], result[2], controlPoints[3] };

		return { curveA, curveB };
	}

	std::vector<mat::Vec3> CurveUtils::FilterKnots(std::vector<mat::Vec3> knots)
	{
		const float eps = static_cast<float>(1e-3);
		std::vector<mat::Vec3> uniqueKnots;
		if (knots.empty())
			return {};

		uniqueKnots.push_back(knots[0]);
		for (int i = 0; i < knots.size() - 1; i++)
		{
			auto chordLength = mat::Length(knots[i + 1] - knots[i]);
			if (chordLength > eps)
				uniqueKnots.push_back(knots[i + 1]);
		}
		return uniqueKnots;
	}

	std::vector<float> CurveUtils::ComputeChordLengths(std::vector<ar::mat::Vec3> knots)
	{
		std::vector<float> chordLenghts;
		for (int i = 0; i < knots.size() - 1; i++)
			chordLenghts.push_back(mat::Length(knots[i + 1] - knots[i]));
		return chordLenghts;
	}

	std::vector<ar::VertexPositionID> CurveUtils::ConvertCoeffToBezier(std::vector<mat::Vec3> coeffA,
		std::vector<mat::Vec3> coeffB, std::vector<mat::Vec3> coeffC, std::vector<mat::Vec3> coeffD,
		std::vector<float> chordLengths, uint32_t id)
	{
		auto verts = std::vector<VertexPositionID>();
		auto n = coeffA.size();
		auto& a = coeffA, &b = coeffB, &c = coeffC, &d = coeffD;

		for (int i = 0; i < n; i++)
		{
			auto cl = chordLengths[i];
			auto b0 = a[i];
			auto b1 = a[i] + (b[i] * cl / 3.0f);
			auto b2 = a[i] + (2 * b[i] * cl / 3.0f) + (c[i] * cl * cl / 3.0f);
			auto b3 = a[i] + b[i] * cl + c[i] * cl * cl + d[i] * cl * cl * cl;
			verts.push_back({ b0, id });
			verts.push_back({ b1, id });
			verts.push_back({ b2, id });
			verts.push_back({ b3, id });
		}
		return verts;
	}

}
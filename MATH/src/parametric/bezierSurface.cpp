#include "bezierSurface.h"
#include <array>

namespace ar::mat
{
	BezierSurface::BezierSurface(std::vector<mat::Vec3d> points, UInt2 segments,
		bool periodicU, bool periodicV)
		: m_Points(points), m_Segments(segments), m_IsPeriodicU(periodicU), 
		m_IsPeriodicV(periodicV), m_Size({ segments.u * 3 + 1, segments.v * 3 + 1 }),
		m_SegWidth(1. / segments.u), m_SegHeight(1. / segments.v)
	{ }

	Vec3d BezierSurface::Evaluate(double u, double v)
	{
		int segU = std::min((int)std::floor(u / m_SegWidth), (int)m_Segments.u - 1);
		int segV = std::min((int)std::floor(v / m_SegHeight), (int)m_Segments.v - 1);
		double localU = std::min(1.0, (u - segU * m_SegWidth) / m_SegWidth);
		double localV = std::min(1.0, (v - segV * m_SegHeight) / m_SegHeight);

		std::array<Vec3d, 4> points, tmp;
		for (size_t row = 0; row < 4; row++)
		{
			size_t baseV = segV * 3;
			size_t baseU = segU * 3;
			size_t index = (baseV + row) * m_Size.u + baseU;

			tmp[0] = m_Points[index];
			tmp[1] = m_Points[index + 1];
			tmp[2] = m_Points[index + 2];
			tmp[3] = m_Points[index + 3];
			points[row] = CubicDeCasteljau(tmp, localU);
		}
		return CubicDeCasteljau(points, localV);
	}
	Vec3d BezierSurface::DerivativeU(double u, double v)
	{
		int segU = std::min((int)std::floor(u / m_SegWidth), (int)m_Segments.u - 1);
		int segV = std::min((int)std::floor(v / m_SegHeight), (int)m_Segments.v - 1);
		double localU = std::min(1.0, (u - segU * m_SegWidth) / m_SegWidth);
		double localV = std::min(1.0, (v - segV * m_SegHeight) / m_SegHeight);

		std::array<mat::Vec3d, 4> points, tmp;
		for (size_t row = 0; row < 4; row++)
		{
			size_t baseV = segV * 3;
			size_t baseU = segU * 3;
			size_t index = (baseV + row) * m_Size.u + baseU;
			tmp[0] = m_Points[index];
			tmp[1] = m_Points[index + 1];
			tmp[2] = m_Points[index + 2];
			tmp[3] = m_Points[index + 3];
			points[row] = CubicBernsteinDerivative(tmp, localU);
		}
		return CubicDeCasteljau(points, localV) / m_SegWidth;
	}
	Vec3d BezierSurface::DerivativeV(double u, double v)
	{
		int segU = std::min((int)std::floor(u / m_SegWidth), (int)m_Segments.u - 1);
		int segV = std::min((int)std::floor(v / m_SegHeight), (int)m_Segments.v - 1);
		double localU = std::min(1.0, (u - segU * m_SegWidth) / m_SegWidth);
		double localV = std::min(1.0, (v - segV * m_SegHeight) / m_SegHeight);

		std::array<mat::Vec3d, 4> points, tmp;
		for (size_t col = 0; col < 4; col++)
		{
			size_t baseV = segV * 3;
			size_t baseU = segU * 3;
			size_t index = (baseU + col);

			tmp[0] = m_Points[index + baseV * m_Size.u];
			tmp[1] = m_Points[index + (baseV + 1) * m_Size.u];
			tmp[2] = m_Points[index + (baseV + 2) * m_Size.u];
			tmp[3] = m_Points[index + (baseV + 3) * m_Size.u];
			points[col] = CubicBernsteinDerivative(tmp, localV);
		}
		return CubicDeCasteljau(points, localU) / m_SegHeight;
	}
	bool BezierSurface::IsPeriodicU() const
	{
		return m_IsPeriodicU;
	}
	bool BezierSurface::IsPeriodicV() const
	{
		return m_IsPeriodicV;
	}
}
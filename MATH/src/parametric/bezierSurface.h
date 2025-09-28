#pragma once
#include "parametricSurface.h"
#include <vector>
#include <array>
#include "solvers.h"

namespace ar::mat
{
	class BezierSurface : public IParametricSurface
	{
	public:
		BezierSurface(std::vector<mat::Vec3d> points, UInt2 segments,
			bool periodicU, bool periodicV);
		Vec3d Evaluate(double u, double v) override;
		Vec3d DerivativeU(double u, double v) override;
		Vec3d DerivativeV(double u, double v) override;
		Vec3d Normal(double u, double v) override;
		bool Clamp(double& u, double& v) override;
		bool IsPeriodicU() const override;
		bool IsPeriodicV() const override;

	private:
		UInt2 m_Segments, m_Size;
		std::vector<mat::Vec3d> m_Points;
		bool m_IsPeriodicU, m_IsPeriodicV;
		double m_SegWidth, m_SegHeight;
	};
}
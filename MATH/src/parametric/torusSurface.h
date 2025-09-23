#pragma once
#include "parametricSurface.h"
#include "vector_types.h"

namespace ar::mat
{
	class TorusSurface : public IParametricSurface
	{
	public:
		TorusSurface(double smallRadius, double largeRadius);
		Vec3d Evaluate(double u, double v) override;
		Vec3d DerivativeU(double u, double v) override;
		Vec3d DerivativeV(double u, double v) override;
		bool IsPeriodicU() const override;
		bool IsPeriodicV() const override;
	
	private:
		double m_SmallRadius, m_LargeRadius;
	};
}
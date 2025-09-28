#pragma once
#include "parametricSurface.h"
#include "vector_types.h"
#include "matrix_types.h"

namespace ar::mat
{
	class TorusSurface : public IParametricSurface
	{
	public:
		TorusSurface(double smallRadius, double largeRadius, Mat4 model);
		Vec3d Evaluate(double u, double v) override;
		Vec3d DerivativeU(double u, double v) override;
		Vec3d DerivativeV(double u, double v) override;
		Vec3d Normal(double u, double v) override;
		bool IsPeriodicU() const override;
		bool IsPeriodicV() const override;
		bool Clamp(double& u, double& v) override;
	
	private:
		double m_SmallRadius, m_LargeRadius;
		Mat4 m_Model;
	};
}
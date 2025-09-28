#pragma once
#include "vector_types.h"

namespace ar::mat
{
	class IParametricSurface
	{
	public:
		virtual ~IParametricSurface() = default;

		virtual Vec3d Evaluate(double u, double v) = 0;
		virtual Vec3d DerivativeU(double u, double v) = 0;
		virtual Vec3d DerivativeV(double u, double v) = 0;
		virtual Vec3d Normal(double u, double v) = 0;
		virtual bool Clamp(double& u, double& v) = 0;
		virtual bool IsPeriodicU() const = 0;
		virtual bool IsPeriodicV() const = 0;
	};
}
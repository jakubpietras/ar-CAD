#include "parametricSurface.h"

namespace ar::mat
{
	class Point : public IParametricSurface
	{
	public:
		Point(Vec3d position);

		Vec3d Evaluate(double u, double v) override;
		Vec3d DerivativeU(double u, double v) override;
		Vec3d DerivativeV(double u, double v) override;
		Vec3d Normal(double u, double v) override;
		bool Clamp(double& u, double& v) override;
		bool IsPeriodicU() const override;
		bool IsPeriodicV() const override;

	private:
		Vec3d m_Position;
	};
}
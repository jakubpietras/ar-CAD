#include "newton.h"
#include "solvers.h"

namespace ar::mat
{
	NewtonSD::NewtonSD(std::shared_ptr<IParametricSurface> first, std::shared_ptr<IParametricSurface> second)
		: m_First(first), m_Second(second)
	{ }
	NewtonResult NewtonSD::Minimize(const Vec4d& initialParams, const Vec3d& startPoint, double d, const NewtonConfig& config)
	{
        NewtonResult result{};
        bool success = false, repeat = false;
        Vec4d params = initialParams, prevParams = initialParams;

        // Compute tangent at initial position (not inside loop)
        Vec3d N1 = m_First->Normal(params.x, params.y);
        Vec3d N2 = m_Second->Normal(params.z, params.w);
        Vec3d T = mat::Cross(N1, N2);

        if (!isfinite(T.x) || !isfinite(T.y) || !isfinite(T.z) || mat::Length(T) < 1e-6)
        {
            T = mat::Normalize(m_First->DerivativeU(params.x, params.y));
            repeat = true;
        }
        else {
            T = mat::Normalize(T);
        }

        for (size_t iter = 0; iter < config.MaxIterations; iter++)
        {
            auto f = Residual(params, startPoint, T, d);
            auto J = Jacobian(params, T);
            auto delta = SolveLinear(J, f);

            prevParams = params;
            params -= delta * config.Damping;

            if (!Clamp(params)) // Went outside the domain
            {
                result.Converged = false;
                result.Solution = prevParams;
                result.Iterations = iter + 1;
                return result;
            }

            if (mat::Dot(delta, delta) < config.Tolerance)
            {
                result.Converged = true;
                result.Solution = params;
                result.Iterations = iter + 1;
                return result;
            }

            if (!isfinite(params.x) || !isfinite(params.y) || !isfinite(params.z) || !isfinite(params.w))
            {
                result.Converged = false;
                result.Solution = prevParams;
                result.Iterations = iter + 1;
                return result;
            }

            // Tangent fallback retry logic
            if (repeat && iter == config.MaxIterations - 1)
            {
                T = mat::Normalize(m_First->DerivativeV(prevParams.x, prevParams.y));
                repeat = false;
                iter = 0;
            }
        }

        result.Solution = params;
        result.Converged = false;
        result.Iterations = config.MaxIterations;
        return result;
	}
	Vec4d NewtonSD::Residual(const Vec4d& params, const Vec3d& startPoint, const Vec3d& T, double d)
	{
		auto p = m_First->Evaluate(params.x, params.y);
		auto q = m_Second->Evaluate(params.z, params.w);
		auto diff = p - q;
		auto midpoint = (p + q) * 0.5;

		return { diff.x, diff.y, diff.z, mat::Dot(midpoint - startPoint, T) - d };
	}
	Mat4d NewtonSD::Jacobian(const Vec4d& params, const Vec3d& T)
	{
		auto dPdu = m_First->DerivativeU(params.x, params.y);
		auto dPdv = m_First->DerivativeV(params.x, params.y);
		auto dQdu = m_Second->DerivativeU(params.z, params.w);
		auto dQdv = m_Second->DerivativeV(params.z, params.w);

		mat::Mat4d j;
		j(0, 0) = dPdu.x;
		j(1, 0) = dPdu.y;
		j(2, 0) = dPdu.z;
		j(3, 0) = 0.5f * mat::Dot(dPdu, T);

		j(0, 1) = dPdv.x;
		j(1, 1) = dPdv.y;
		j(2, 1) = dPdv.z;
		j(3, 1) = 0.5f * mat::Dot(dPdv, T);

		j(0, 2) = -dQdu.x;
		j(1, 2) = -dQdu.y;
		j(2, 2) = -dQdu.z;
		j(3, 2) = 0.5f * mat::Dot(dQdu, T);

		j(0, 3) = -dQdv.x;
		j(1, 3) = -dQdv.y;
		j(2, 3) = -dQdv.z;
		j(3, 3) = 0.5f * mat::Dot(dQdv, T);

		return j;
	}
	bool NewtonSD::Clamp(Vec4d& params)
	{
		auto c1 = m_First->Clamp(params.x, params.y);
		auto c2 = m_Second->Clamp(params.z, params.w);
		return c1 && c2;
	}
}
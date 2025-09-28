#pragma once
#include <memory>
#include "parametric/parametricSurface.h"
#include "vector_types.h"
#include "matrix_types.h"

namespace ar::mat
{
    struct NewtonResult
    {
        Vec4d   Solution{};
        bool    Converged = false;
        size_t  Iterations = 0;
    };

    struct NewtonConfig
    {
        double  Tolerance = 1e-6;
        size_t  MaxIterations = 30;
        double  Damping = 0.1;
    };

    class NewtonSD
    {
    public:
        NewtonSD(std::shared_ptr<IParametricSurface> first,
            std::shared_ptr<IParametricSurface> second);

        NewtonResult Minimize(const Vec4d& initialParams, const Vec3d& fixedStartPoint, double d, const NewtonConfig& config = {});

    private:
        std::shared_ptr<IParametricSurface> m_First, m_Second;

        // builds residuals & Jacobian for the squared distance system
        Vec4d Residual(const Vec4d& params, const Vec3d& startPoint, const Vec3d& T, double d);
        Mat4d Jacobian(const Vec4d& params, const Vec3d& T);

        bool Clamp(Vec4d& params);
    };
}
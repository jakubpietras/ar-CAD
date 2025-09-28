#include "torusSurface.h"
#include <numbers>

namespace ar::mat
{
    TorusSurface::TorusSurface(double smallRadius, double largeRadius, Mat4 model)
        : m_SmallRadius(smallRadius), m_LargeRadius(largeRadius), m_Model(model)
    { }

    Vec3d TorusSurface::Evaluate(double u, double v)
    {
        double twoPi = 2.f * std::numbers::pi;
        double theta = u * twoPi, phi = v * twoPi;

        auto x = (m_LargeRadius + m_SmallRadius * cos(phi)) * cos(theta);
        auto y = m_SmallRadius * sin(phi);
        auto z = (m_LargeRadius + m_SmallRadius * cos(phi)) * sin(theta);

        auto pt = mat::Vec4(x, y, z, 1.);
        auto res = m_Model * pt;

        return { res.x, res.y, res.z };
    }

    Vec3d TorusSurface::DerivativeU(double u, double v)
    {
        double twoPi = 2 * std::numbers::pi;
        double phi = v * twoPi, theta = u * twoPi;

        auto dxdu = -sin(theta) * (m_LargeRadius + m_SmallRadius * cos(phi)) * twoPi;
        auto dydu = 0.f;
        auto dzdu = cos(theta) * (m_LargeRadius + m_SmallRadius * cos(phi)) * twoPi;

        auto pt = mat::Vec4(dxdu, dydu, dzdu, 0.);
        auto res = m_Model * pt;

        return { res.x, res.y, res.z };
    }

    Vec3d TorusSurface::DerivativeV(double u, double v)
    {
        double twoPi = 2 * std::numbers::pi;
        double theta = u * twoPi, phi = v * twoPi;

        auto dxdv = -m_SmallRadius * cos(theta) * sin(phi) * twoPi;
        auto dydv = m_SmallRadius * cos(phi) * twoPi;
        auto dzdv = -m_SmallRadius * sin(phi) * sin(theta) * twoPi;
        
        auto pt = mat::Vec4(dxdv, dydv, dzdv, 0.);
        auto res = m_Model * pt;

        return { res.x, res.y, res.z };
    }

    bool TorusSurface::IsPeriodicU() const { return true; }
    bool TorusSurface::IsPeriodicV() const { return true; }

    bool TorusSurface::Clamp(double& u, double& v)
    {
        auto wrap = [](double& x) -> bool {
            if (x < 0.0 || x >= 1.0) {
                x = x - std::floor(x);
                if (x >= 1.0) x = 0.0;  // Handle edge case
            }
            return true;  // Always valid for torus
            };

        wrap(u);
        wrap(v);
        return true;  // Torus is always valid after wrapping
    }
    Vec3d TorusSurface::Normal(double u, double v)
    {
        auto du = DerivativeU(u, v);
        auto dv = DerivativeV(u, v);
        return mat::Normalize(mat::Cross(du, dv));
    }
}

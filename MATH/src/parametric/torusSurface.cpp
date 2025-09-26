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
        return { x, y, z };
    }

    Vec3d TorusSurface::DerivativeU(double u, double v)
    {
        double twoPi = 2 * std::numbers::pi;
        double phi = v * twoPi, theta = u * twoPi;

        auto dxdu = -sin(theta) * (m_LargeRadius + m_SmallRadius * cos(phi)) * twoPi;
        auto dydu = 0.f;
        auto dzdu = cos(theta) * (m_LargeRadius + m_SmallRadius * cos(phi)) * twoPi;
        return { dxdu, dydu, dzdu };
    }

    Vec3d TorusSurface::DerivativeV(double u, double v)
    {
        double twoPi = 2 * std::numbers::pi;
        double theta = u * twoPi, phi = v * twoPi;

        auto dxdv = -m_SmallRadius * cos(theta) * sin(phi) * twoPi;
        auto dydv = m_SmallRadius * cos(phi) * twoPi;
        auto dzdv = -m_SmallRadius * sin(phi) * sin(theta) * twoPi;
        return { dxdv, dydv, dzdv };
    }

    bool TorusSurface::IsPeriodicU() const { return true; }

    bool TorusSurface::IsPeriodicV() const { return true; }
    bool TorusSurface::Clamp(double& u, double& v)
    {
        auto wrap = [](double& x) {
            if (x < 0.0 || x >= 1.0) {
                x = x - std::floor(x);   // Wrap into [0,1)
            }
            // Always valid for a torus
            return true;
            };
        bool okU = wrap(u);
        bool okV = wrap(v);
        return okU && okV;
    }
}

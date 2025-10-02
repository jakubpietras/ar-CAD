#include "point.h"

namespace ar::mat
{
    Point::Point(Vec3d position)
        : m_Position(position)
    { }
    Vec3d ar::mat::Point::Evaluate(double u, double v)
    {
        return m_Position;
    }

    Vec3d ar::mat::Point::DerivativeU(double u, double v)
    {
        return 0.0;
    }

    Vec3d ar::mat::Point::DerivativeV(double u, double v)
    {
        return 0.0;
    }

    Vec3d ar::mat::Point::Normal(double u, double v)
    {
        return { 0., 0., 0. };
    }

    bool ar::mat::Point::Clamp(double& u, double& v)
    {
        return true;
    }

    bool ar::mat::Point::IsPeriodicU() const
    {
        return false;
    }

    bool ar::mat::Point::IsPeriodicV() const
    {
        return false;
    }
}


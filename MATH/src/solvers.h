#pragma once
#include <vector>
#include "vector_types.h"

namespace ar
{
    std::vector<float> SolveTridiagonal(
        const std::vector<float>& lower,
        const std::vector<float>& main,
        const std::vector<float>& upper,
        const std::vector<float>& rhs);
    std::vector<Vec3> SolveTridiagonal(
        const std::vector<float>& lower,
        const std::vector<float>& main,
        const std::vector<float>& upper,
        const std::vector<Vec3>& rhs);
}
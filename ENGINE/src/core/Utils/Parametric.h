#pragma once
#include "core/Core.h"
#include "parametric/parametricSurface.h"
#include "parametric/torusSurface.h"
#include "parametric/bezierSurface.h"
#include "core/Scene/Entity.h"

namespace ar
{
	class Parametric
	{
	public:
		static Ref<mat::IParametricSurface> Create(Entity entity);
		static Ref<mat::IParametricSurface> Create(ar::mat::Vec3d position);

	private:
		static std::vector<mat::Vec3d> GetBezierPoints(Entity entity);
	};
}
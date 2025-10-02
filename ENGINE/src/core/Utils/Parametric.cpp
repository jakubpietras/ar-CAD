#include "arpch.h"
#include "Parametric.h"
#include "core/Scene/Components.h"
#include "core/Utils/GeneralUtils.h"
#include "parametric/point.h"

namespace ar
{
	Ref<mat::IParametricSurface> Parametric::Create(Entity entity)
	{
		if (entity.HasComponent<ar::TorusComponent>())
		{
			auto& desc = entity.GetComponent<ar::TorusComponent>().Description;
			auto& model = entity.GetComponent<ar::TransformComponent>().ModelMatrix;
			return std::make_shared<mat::TorusSurface>(desc.SmallRadius, desc.LargeRadius, model);
		}
		if (entity.HasComponent<ar::SurfaceComponent>())
		{
			auto points = GetBezierPoints(entity);
			auto& desc = entity.GetComponent<ar::SurfaceComponent>().Description;
			bool isPeriodicU = 
				(desc.Type == SurfaceType::CYLINDERC0 || desc.Type == SurfaceType::CYLINDERC2);

			return std::make_shared<mat::BezierSurface>(points, desc.Segments, isPeriodicU, false);
		}
		return nullptr;
	}
	Ref<mat::IParametricSurface> Parametric::Create(ar::mat::Vec3d position)
	{
		return std::make_shared<mat::Point>(position);
	}
	std::vector<mat::Vec3d> Parametric::GetBezierPoints(Entity entity)
	{
		// C0 - just retrieve positions
		auto& desc = entity.GetComponent<ar::SurfaceComponent>().Description;
		if (desc.Type == SurfaceType::RECTANGLEC0 || desc.Type == SurfaceType::CYLINDERC0)
		{
			auto& pts = entity.GetComponent<ar::ControlPointsComponent>().Points;
			return GeneralUtils::GetPosD(pts);
		}
		// C2 - convert DeBoor points into Bezier (Bernstein)
		return SurfaceUtils::GetBezierFromDeBoorD(entity);
	}
}
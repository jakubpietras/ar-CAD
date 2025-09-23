#include "arpch.h"
#include "Parametric.h"
#include "core/Scene/Components.h"

namespace ar
{
	Ref<mat::IParametricSurface> Parametric::Create(Entity entity)
	{
		if (entity.HasComponent<ar::TorusComponent>())
		{
			auto& desc = entity.GetComponent<ar::TorusComponent>().Description;
			return std::make_shared<mat::TorusSurface>(desc.SmallRadius, desc.LargeRadius);
		}
		return nullptr;
	}
}